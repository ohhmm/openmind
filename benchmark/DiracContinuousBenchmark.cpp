#include "omnn/quantum/DiracClient.h"

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

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <problem.json> [device] [poll_ms] [timeout_ms]\n";
        return 2;
    }

    const std::string problemPath = argv[1];
    const std::string device = (argc > 2) ? argv[2] : "dirac-3";
    const int pollMs = (argc > 3) ? std::stoi(argv[3]) : 1000;
    const int timeoutMs = (argc > 4) ? std::stoi(argv[4]) : 300000;

    try {
        const std::string problemJson = readFile(problemPath);
        omnn::quantum::DiracClient client;

        auto t0 = steady_clock_t::now();
        const std::string uploadResponse = client.UploadProblem(problemJson);
        auto tUpload = steady_clock_t::now();

        const std::string problemId = [&]() {
            std::string needle = "\"file_id\"";
            size_t pos = uploadResponse.find(needle);
            if (pos == std::string::npos) throw std::runtime_error("upload response missing file_id");
            pos = uploadResponse.find(':', pos);
            if (pos == std::string::npos) throw std::runtime_error("upload response malformed");
            size_t valueStart = uploadResponse.find_first_not_of(" \t\r\n", pos + 1);
            if (valueStart == std::string::npos || uploadResponse[valueStart] != '"') throw std::runtime_error("upload response file_id not string");
            size_t end = uploadResponse.find('"', valueStart + 1);
            if (end == std::string::npos) throw std::runtime_error("upload response file_id unterminated");
            return uploadResponse.substr(valueStart + 1, end - valueStart - 1);
        }();

        const std::string createResponse = client.CreateJob(problemId, device);
        auto tCreateTmp = steady_clock_t::now();
        (void)tCreateTmp;

        const std::string jobId = [&]() {
            std::string needle = "\"job_id\"";
            size_t pos = createResponse.find(needle);
            if (pos == std::string::npos) {
                // try "id" instead
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
        auto tCreate = tCreateTmp;

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
