#include "omnn/quantum/DiracClient.h"

#include <boost/json.hpp>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using steady_clock_t = std::chrono::steady_clock;

static std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("cannot open input file: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static std::string toQciContinuousFile(const std::string& inputJson) {
    namespace json = boost::json;
    json::value v = json::parse(inputJson);
    if (!v.is_object()) throw std::runtime_error("input JSON must be an object");
    const json::object& root = v.as_object();

    // Already a QCI file dict: pass it through.
    if (root.contains("file_config")) {
        return inputJson;
    }

    if (!root.contains("linear") || !root.contains("quadratic")) {
        throw std::runtime_error("input JSON must contain either file_config, or linear+quadratic coefficients");
    }

    const json::array& linear = root.at("linear").as_array();
    const json::array& quadratic = root.at("quadratic").as_array();
    int numVars = static_cast<int>(linear.size());
    if (numVars == 0 || numVars != static_cast<int>(quadratic.size())) {
        throw std::runtime_error("linear and quadratic dimensions must match and be non-empty");
    }

    json::array data;
    // Convert linear terms to the diagonal of the Hamiltonian matrix.
    for (int i = 0; i < numVars; ++i) {
        double value = linear[i].is_double() ? linear[i].as_double() : static_cast<double>(linear[i].as_int64());
        data.push_back(json::object{{"i", i}, {"j", i}, {"val", value}});
    }
    // Add quadratic off-diagonal contributions.
    for (int i = 0; i < numVars; ++i) {
        const json::array& row = quadratic[i].as_array();
        if (static_cast<int>(row.size()) != numVars) {
            throw std::runtime_error("quadratic matrix must be square");
        }
        for (int j = i + 1; j < numVars; ++j) {
            double value = row[j].is_double() ? row[j].as_double() : static_cast<double>(row[j].as_int64());
            if (value != 0.0) {
                data.push_back(json::object{{"i", i}, {"j", j}, {"val", value}});
                data.push_back(json::object{{"i", j}, {"j", i}, {"val", value}});
            }
        }
    }

    json::object typeConfig;
    typeConfig["num_variables"] = numVars;
    typeConfig["data"] = data;

    json::object fileConfig;
    fileConfig["hamiltonian"] = typeConfig;

    json::object converted;
    if (root.contains("file_name") && root.at("file_name").is_string()) {
        converted["file_name"] = root.at("file_name").as_string();
    } else {
        converted["file_name"] = "converted_continuous_problem.json";
    }
    converted["file_config"] = fileConfig;
    return json::serialize(converted);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <problem.json|coefficients.json> [device] [poll_ms] [timeout_ms]\n";
        return 2;
    }

    const std::string inputPath = argv[1];
    const std::string device = (argc > 2) ? argv[2] : "dirac-3";
    const int pollMs = (argc > 3) ? std::stoi(argv[3]) : 1000;
    const int timeoutMs = (argc > 4) ? std::stoi(argv[4]) : 30000;

    try {
        const std::string inputJson = readFile(inputPath);
        const std::string problemJson = toQciContinuousFile(inputJson);
        omnn::quantum::DiracClient client;

        auto t0 = steady_clock_t::now();
        const std::string uploadResponse = client.UploadProblem(problemJson);
        auto tUpload = steady_clock_t::now();

        const std::string problemId = [&]() {
            const std::string needle = "\"file_id\"";
            size_t pos = uploadResponse.find(needle);
            if (pos == std::string::npos) throw std::runtime_error("upload response missing file_id: " + uploadResponse);
            pos = uploadResponse.find(':', pos);
            if (pos == std::string::npos) throw std::runtime_error("upload response malformed: " + uploadResponse);
            size_t valueStart = uploadResponse.find_first_not_of(" \t\r\n", pos + 1);
            if (valueStart == std::string::npos || uploadResponse[valueStart] != '"') throw std::runtime_error("upload response file_id not string: " + uploadResponse);
            size_t end = uploadResponse.find('"', valueStart + 1);
            if (end == std::string::npos) throw std::runtime_error("upload response file_id unterminated: " + uploadResponse);
            return uploadResponse.substr(valueStart + 1, end - valueStart - 1);
        }();

        const std::string createResponse = client.CreateJob(problemId, device);
        auto tCreate = steady_clock_t::now();

        const std::string jobId = [&]() {
            std::string needle = "\"job_id\"";
            size_t pos = createResponse.find(needle);
            if (pos == std::string::npos) {
                const std::string needle2 = "\"id\"";
                pos = createResponse.find(needle2);
                if (pos == std::string::npos) throw std::runtime_error("create response missing job_id/id: " + createResponse);
                needle = needle2;
            }
            pos = createResponse.find(':', pos);
            if (pos == std::string::npos) throw std::runtime_error("create response malformed: " + createResponse);
            size_t valueStart = createResponse.find_first_not_of(" \t\r\n", pos + 1);
            if (valueStart == std::string::npos || createResponse[valueStart] != '"') throw std::runtime_error("create response id not string: " + createResponse);
            size_t end = createResponse.find('"', valueStart + 1);
            if (end == std::string::npos) throw std::runtime_error("create response id unterminated: " + createResponse);
            return createResponse.substr(valueStart + 1, end - valueStart - 1);
        }();

        std::string status;
        while (true) {
            status = client.PollJobStatus(jobId);
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock_t::now() - t0).count();
            std::cout << "status=" << status << " elapsed_ms=" << elapsed << "\n";
            if (status == "COMPLETED" || status == "FAILED" || status == "CANCELLED" || status == "ERRORED" || status == "ERROR") break;
            if (elapsed >= timeoutMs) throw std::runtime_error("timeout waiting for job");
            std::this_thread::sleep_for(std::chrono::milliseconds(pollMs));
        }
        auto tDone = steady_clock_t::now();

        const std::string result = client.GetJobResult(jobId);
        auto tResult = steady_clock_t::now();

        std::cout << "problem_id=" << problemId << "\n";
        std::cout << "job_id=" << jobId << "\n";
        std::cout << "final_status=" << status << "\n";
        std::cout << "upload_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(tUpload - t0).count() << "\n";
        std::cout << "create_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(tCreate - tUpload).count() << "\n";
        std::cout << "solve_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(tDone - tCreate).count() << "\n";
        std::cout << "result_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(tResult - tDone).count() << "\n";
        std::cout << "total_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(tResult - t0).count() << "\n";
        std::cout << "result=\n" << result << "\n";
        return status == "COMPLETED" ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "benchmark error: " << e.what() << "\n";
        return 1;
    }
}
