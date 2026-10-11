
#include "Valuable.h"
#include <skrypt.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/dll/shared_library.hpp>

#include <fstream>
#include <iostream>
#include <string>


using namespace skrypt;
using namespace omnn::math;

using namespace std::literals;


void Skrypt::SetVarhost(decltype(varHost) host)
{
	auto allocatedVarsCount = varHost->Stored();
	if(allocatedVarsCount != 0) {
		LOG_AND_IMPLEMENT("VarHost has " << allocatedVarsCount << " vars allocated. Consider moving thouse to the new host. The base system needs the support.")
	}else{
		varHost = host;
	}
}

bool Skrypt::ParseTotal(std::istream& in, std::string_view& line, std::function<void(Valuable&&)> result)
{
	MakesTotalEqu(true);
	auto parsingIsOnTheGo = ParseNextLine(in, line);
	if (parsingIsOnTheGo)
		Load(in);
	if (DisjunctionParseMode())
		result(std::move(disjunction));
	else if (!Expressions().empty()) {
		auto& total = const_cast<::omnn::math::Valuable&>(Total());
		result(std::move(total));
	}
	return parsingIsOnTheGo;
}

const Variable& Skrypt::MappedModuleVariable(const Variable& v, module_t module) {
    auto name = GetVariableName(v);
    auto moduleName = GetModuleName(name);
    if (!moduleName.empty()) {
        name.remove_prefix(moduleName.size() + 1 // '.'
        );
        if (!module) {
            module = Module(moduleName);
        } else if (IsModuleLoading(moduleName)) {
            if(module != WaitTillModuleLoadingComplete(moduleName))
                LOG_AND_IMPLEMENT("Module provided does not match");
        }
        auto& moduleVarNames = module->GetVarNames();
        auto moduleVarIt = moduleVarNames.find(name);
        if (moduleVarIt != moduleVarNames.end()) {
            return moduleVarIt->second;
        } else {
            std::shared_lock lock(modulesLoadingMutex);
            auto loading = modulesLoading.find(moduleName);
            auto isModuleLoading = loading != modulesLoading.end();
            if (isModuleLoading) {
                auto loadingModule = loading->second;
                if (module != loadingModule) {
                    LOG_AND_IMPLEMENT(
                        "Either variable of different module or resolve race condition in module loading");
                } else {
                    auto moduleVarIt = module->GetVarNames().find(name);
                    if (moduleVarIt != module->GetVarNames().end()) {
                        return moduleVarIt->second;
                    } else {
                        LOG_AND_IMPLEMENT("Variable not found in the module");
                    }
                }
            } else {
                LOG_AND_IMPLEMENT("Module not found");
            }
        }
    }
    return v; // this module variable
}

void Skrypt::PrintVarKnowns(const Variable& v)
{
    auto module = Module(v);
    auto mappedModuleVariable = MappedModuleVariable(v, module);
    if (mappedModuleVariable.getVaHost() != v.getVaHost() || mappedModuleVariable != v) {
        std::cout << GetModuleName(v) << '.';
        module->PrintVarKnowns(mappedModuleVariable);
        return;
    }
	std::cout << v << " =";
	auto solutions = Solve(v);
	for (auto& solution : solutions) {
		std::cout << ' ' << solution;
	}
	std::cout << std::endl;
}

void Skrypt::PrintAllKnowns()
{
    for (auto& [name, var] : vars) {
        PrintVarKnowns(var);
    }
}

void Skrypt::BindTargetStream(std::ostream& o) {
    outputs.emplace_back(&o, [](auto){});
}

void Skrypt::BindTargetStream(std::shared_ptr<std::ostream> o) {
    outputs.emplace_back(std::move(o));
}

void Skrypt::BindTargetStream(const boost::filesystem::path& filepath){
    BindTargetStream(std::make_shared<boost::filesystem::ofstream>(filepath));
}

bool Skrypt::Add(Valuable&& v) {
    BackgroundLoadingModules(v);
    v.CollectVaNames(vars);
    bool ok = {};
    try
    {
        if (v.IsVa()) {
            PrintVarKnowns(v.as<Variable>());
            ok = true;
        } else {
            if(echo)
                std::cout << v << std::endl;
            if (ok && outputs.size()) {
                for (auto& o : outputs) {
                    *o << v << std::endl;
                }
            }
            if (DisjunctionParseMode())
                ok = disjunction.Add(std::move(v)) != disjunction.end();
            else
                ok = base::Add(std::move(v));
        }
    } catch (...) {
        if (echo && !ok) {
            std::cout << "Please, consider additional defining to the system to make this request" << std::endl;
        }
    }
    return ok;
}

bool Skrypt::Add(std::string_view line) {
	Valuable v(line, varHost);
    return Add(v);
}

namespace {
	std::string Questionless(std::string s) {
		s.erase(std::remove(s.begin(), s.end(), '?'), s.end());
		return s;
	}
	std::string Questionless(std::string_view s) {
		return Questionless(std::string(s));
	}
	auto FindBracePos(std::string_view line) {
		return line.find_first_of("{}[]"sv);
	}
}

void Skrypt::ProcessQuestionLine(std::string_view& line)
{
    YesNoMaybe is = YesNoMaybe::Maybe;
    auto questionless = Questionless(line);
    if (questionless.empty()) {
        PrintAllKnowns();
    }
	else {
        Valuable expression(questionless, varHost);
        expression.SetView(Valuable::View::Equation);
        auto lineVars = expression.Vars();
        Valuable::vars_cont_t knowns;
        for (auto& v : lineVars) {
            auto& known = Known(v);
            if (known.size() == 1) {
                knowns.emplace(v, *known.begin());
            }
        }

        if (expression.IsSimple()) {
            if (expression.IsZero()) {
                is = YesNoMaybe::Yes;
            } else if (expression.Distinct().contains(constants::zero)) {
                std::cout << "potentially ";
                is = YesNoMaybe::Yes;
            }
			else {
                is = YesNoMaybe::No;
            }
        } else if (knowns.size()) {
            expression.eval(knowns);
            expression.optimize();
            if (expression.IsInt()) {
                if (expression.IsZero()) {
                    is = YesNoMaybe::Yes;
                } else {
                    is = YesNoMaybe::No;
                }
            } else if (expression.IsSimple()) {
                if (expression.Distinct().contains(constants::zero)) {
                    std::cout << "not neccessarily, but ";
                    is = YesNoMaybe::Yes;
                } else {
                    is = YesNoMaybe::No;
                }
            } else {
                LOG_AND_IMPLEMENT(expression);
            }
        } else
            try {
                const auto total = MakesTotalEqu() ? Total().Link() : CalculateTotalExpression();
                if (total.IsSum()) {
                    auto rest = total / expression;
                    std::cout << "Total: " << total << std::endl
                              << total << " / " << expression << ": " << rest << std::endl;
                    auto& totalSum = total.as<Sum>();
                    if (lineVars.size() == 1) {
                        auto& va = *lineVars.begin();
                        std::vector<Valuable> coefficients;
                        auto totalGrade = totalSum.FillPolynomialCoefficients(coefficients, va);
                        coefficients.clear();
                        if (expression.IsSum()) {
                            auto& lineSum = expression.as<Sum>();
                            auto lineGrade = lineSum.FillPolynomialCoefficients(coefficients, va);
                            if (rest.IsSum()) {
                                auto restGrade = rest.as<Sum>().FillPolynomialCoefficients(coefficients, va);
                                if (totalGrade == restGrade + lineGrade)
                                    is = YesNoMaybe::Yes;
                            }
                        } else if (expression.IsVa()) {
                            auto solutions = Solve(expression.as<Variable>());
                            if (solutions.size() == 1) {
                                if (solutions.cbegin()->IsZero())
                                    is = YesNoMaybe::Yes;
                            }
                        }
                    } else {
                        IMPLEMENT
                    }
                } else if (total.IsZero()) {
                } else if (total.IsInt()) {
                    is = YesNoMaybe::No;
                } else {
                    IMPLEMENT
                }
            } catch (...) {
                is = YesNoMaybe::Maybe;
            }

        std::cout << '\n' << expression << " ?\n";
        if (is == YesNoMaybe::Yes)
            std::cout << "YES";
        else if (is == YesNoMaybe::No)
            std::cout << "NO";
        else
            std::cout << "IDK";
        std::cout << std::endl;
    }
}

bool Skrypt::ParseNextLine(std::istream& in, std::string_view& line)
{
    auto comment = line.find_first_of('#');
    if (comment != std::string_view::npos)
        line.remove_suffix(comment);
    if (!line.empty()) {
        auto bracePos = FindBracePos(line);
        auto brace = bracePos == std::string_view::npos ? 0 : line[bracePos];
        line.remove_prefix(bracePos + 1);
        switch (brace) {
        case '{': {
            if (disjunctionParseMode) {
                Skrypt subscript;
                subscript.SetVarhost(GetVarHost());
                subscript.Echo(echo);
                return subscript.ParseTotal(in, line,
                                            [this](::omnn::math::Valuable&& result) { Add(std::move(result)); });
            } else if (Expressions().empty()) {
                if (line.empty())
                    return true;
                else
                    break;
            } else {
                LOG_AND_IMPLEMENT("No need for double conjunction");
            }
        }
        case '}': {
            if (disjunctionParseMode) {
                LOG_AND_IMPLEMENT("Closing disjunction with conjunction brace");
            }
            return {};
        }
        case '[': {
            if (disjunctionParseMode) {
                LOG_AND_IMPLEMENT("No need for double disjunction");
            }
            Skrypt subscript;
            subscript.SetVarhost(GetVarHost());
            subscript.Echo(echo);
            subscript.DisjunctionParseMode(true);
            return subscript.ParseTotal(in, line, [this](::omnn::math::Valuable&& result) { Add(std::move(result)); });
        }
        case ']': {
            if (DisjunctionParseMode()) {
                Add(std::move(disjunction));
            } else {
                LOG_AND_IMPLEMENT("Closing conjunction with disjunction brace");
            }
            return {};
        }
        case 0: {
            break;
        }
        default: {
            IMPLEMENT;
        }
        }

        if (boost::algorithm::contains(line, "?")) {
            ProcessQuestionLine(line);
        } else {
            Valuable expression;
            try {
                expression = Valuable(line, varHost);
            } catch (std::runtime_error error) {
                std::cout << "Expression parsing error: " << error.what() << std::endl;
            } catch (...) {
                std::cout << "Expression parsing error." << std::endl;
            }
            if (expression.IsSimple()) {
                std::cout << expression << std::endl;
            } else {
                Add(std::move(expression));
            }
        }
        line = {};
    }
    return true;
}

const Valuable::va_names_t& Skrypt::Load(std::istream& in)
{
	std::string line;
	while (std::getline(in, line)) {
		std::string_view substr = line;
		if (ParseNextLine(in, substr)) {
			if (substr.size()) {
				LOG_AND_IMPLEMENT("Multiple braces per line:" << substr);
			}
		}
		else {
			break;
		}
	}
	return vars;
}

const Valuable::va_names_t& Skrypt::Load(const boost::filesystem::path & path)
{
	std::cout << "Loading " << path << '\n' << std::endl;
    auto filepath = boost::filesystem::exists(path)
        ? path
        : FindModulePath({path.string()});
    boost::filesystem::ifstream stream(filepath);
	return Load(stream);
}

Skrypt::Skrypt(const boost::filesystem::path & path)
{
	sourceFilePath = path;
	Load(path);
}

Skrypt::Skrypt(std::istream & stream)
{
	MakesTotalEqu(true);
	Load(stream);
}

boost::filesystem::path Skrypt::FindModulePath(std::string_view name) const {
    auto path = sourceFilePath.parent_path() / name;
    if (!path.has_extension())
        path.replace_extension(".skrypt");
    if (!boost::filesystem::exists(path)) {
        path = path.filename();
    }
    auto base = boost::dll::program_location();
    if (!boost::filesystem::exists(path)) {
        path = base.parent_path() / name;
        if (!path.has_extension())
            path.replace_extension(".skrypt");
    }

    if (!boost::filesystem::exists(path)) {
        auto loc = boost::dll::this_line_location();
        if (loc != base) {
            path = loc.parent_path() / name;
            if (!path.has_extension())
                path.replace_extension(".skrypt");
        }
    }

    if (!boost::filesystem::exists(path)) {
        for (auto& searchPath : moduleFileSearchAdditionalPaths) {
			path = searchPath / name;
			if (!path.has_extension())
				path.replace_extension(".skrypt");
            if (boost::filesystem::exists(path)) {
				break;
			}
		}
    }

    if (!boost::filesystem::exists(path)) {
        std::cerr << "Module not found: " << path << std::endl;
    }
    return path;
}

Skrypt::module_t Skrypt::GetLoadedModule(std::string_view fileName) {
    module_t loaded;
    std::shared_lock lock(modulesMapMutex);
    auto it = modules.find(fileName);
    if (it != modules.end()) {
        loaded = it->second;
    }
    return loaded;
}

bool Skrypt::IsModuleLoading(std::string_view name) {
    std::shared_lock lock(modulesLoadingMutex);
    return modulesLoading.contains(name);
}

void Skrypt::WaitAllModulesLoadingComplete() {
    while (!modulesLoadingQueue.empty()) {
        modulesLoadingQueue.PeekNextResult();
    }
}

Skrypt::module_t Skrypt::Module(std::string_view name) {
    auto module = GetLoadedModule(name);
    if (!module) {
		auto isCurrentlyLoading = IsModuleLoading(name);
        if (isCurrentlyLoading) {
			std::unique_lock lock(modulesLoadingMutex);
			auto loading = modulesLoading.find(std::string(name));
			if (loading != modulesLoading.end()) {
				module = loading->second;
            } else {
				// probably loaded in the meantime
				module = GetLoadedModule(name);
            }
        }
        if (!module) {
			std::unique_lock lock(modulesLoadingMutex);
            module = std::make_shared<Skrypt>();
            auto insertedLoadingModules = modulesLoading.emplace(name, module);
			auto inserted = insertedLoadingModules.second;
            isCurrentlyLoading = !inserted;
            if (isCurrentlyLoading) {
				module = insertedLoadingModules.first->second;
			}
        }
        {
			std::unique_lock lock(modulesMapMutex);
            auto insertedToModules = modules.emplace(name, module);
            auto inserted = insertedToModules.second;
            if (!inserted) {
				module = insertedToModules.first->second;
			}
		}
        if (!isCurrentlyLoading) {
            module->Echo({});
            module->moduleFileSearchAdditionalPaths = moduleFileSearchAdditionalPaths;
            module->Load(FindModulePath(name));

			std::unique_lock lock(modulesLoadingMutex);
            auto loading = modulesLoading.find(name);
            if (loading != modulesLoading.end()) {
				modulesLoading.erase(loading);
            } else {
                std::cerr << "Module " << name << " was not found in the loading map" << std::endl; // race condition
            }
        } else {
            WaitTillModuleLoadingComplete(name);
        }
    }
    return module;
}

Skrypt::loading_module_t Skrypt::StartLoadingModule(std::string_view moduleName) {
    std::cout << "Module " << moduleName << " loading started" << std::endl;
    return std::async(std::launch::async, [this, moduleName]() {
        auto module = Module(moduleName);
        std::cout << "Module " << moduleName << " loaded:" << std::endl;
        module->PrintAllKnowns();

        // Import known variables from the module TODO: through Binder
        const auto& moduleVarNames = module->GetVarNames();
        for (auto& [name, var] : moduleVarNames) {
            auto& knowns = module->Known(var);
            auto knownSolutionsNumber = knowns.size();
            if (knownSolutionsNumber) {
                std::string varName(moduleName);
                varName += '.';
                varName += name;
                auto& v = varHost->Host(varName);
                if (knownSolutionsNumber == 1)
                    Add(v, *knowns.begin());
                else {
                    auto& solutions = Yarns(v)[{}];
                    for (auto& solution : knowns) {
                        solutions.insert(solution);
					}
                }
            }
        }
        return module;
    });
}

Skrypt::loading_modules_t Skrypt::LoadModules(const ::omnn::math::Valuable& v) {
    loading_modules_t loadingModules;
    for (auto& [name, var] : v.VaNames()) {
        auto dot = name.find('.');
        if (dot != std::string::npos) {
            auto moduleName = name.substr(0, dot);
            auto loading = loadingModules.find(std::string(moduleName));
            if (loading == loadingModules.end()) { // not loading here yet
                auto loaded = GetLoadedModule(moduleName);
                if (loaded)
                {
                    std::promise<module_t> promise;
                    promise.set_value(loaded);
                    loadingModules.emplace(moduleName, promise.get_future());
                } else {
                    loadingModules.emplace(moduleName, StartLoadingModule(moduleName));
                }
            }
        }
    }
    return loadingModules;
}

Skrypt::loading_modules_future_t Skrypt::StartLoadingModules(const Valuable& v) {
    return std::async(std::launch::async, &Skrypt::LoadModules, this, v);
}

void Skrypt::BackgroundLoadingModules(const ::omnn::math::Valuable& v) {
    modulesLoadingQueue.AddTask(&Skrypt::LoadModules, this, v);
}

std::string_view Skrypt::GetVariableName(const ::omnn::math::Variable& v) const {
    auto host = v.getVaHost();
    auto name = host->GetName(v.getId());
    return name;
}

std::string_view Skrypt::GetModuleName(std::string_view name) const {
    auto dot = name.find('.');
    if (dot == std::string::npos) {
        name = {};
    } else {
        name.remove_suffix(name.size() - dot);
    }
    return name;
}

std::string_view Skrypt::GetModuleName(const ::omnn::math::Variable& v) const {
    auto name = GetVariableName(v);
    return GetModuleName(name);
}

Skrypt::module_t Skrypt::Module(const ::omnn::math::Variable& v) {
    auto name = GetVariableName(v);
    auto moduleName = GetModuleName(name);
    if (moduleName.empty()) {
        return {};
    } else
        return Module(moduleName);
}

Skrypt::module_t Skrypt::WaitTillModuleLoadingComplete(std::string_view name) {
    bool isModuleLoading = IsModuleLoading(name);
    if (isModuleLoading) {
        do {
            if (isModuleLoading) {
                auto loaded = modulesLoadingQueue.PeekNextResult();
                auto it = loaded.find(name);
                if (it != loaded.end()) {
                    return it->second.get();
                }
                isModuleLoading = IsModuleLoading(name);
                if (isModuleLoading) {
                    std::this_thread::yield();
                } else {
                    break;
                }
            }
        } while (isModuleLoading);
    }
    return Module(name);
}

const Valuable::solutions_t& Skrypt::Known(const Variable& v)
{
	auto known = std::cref(base::Known(v));
    if (known.get().size() == 0) {
        auto module = Module(v);
        auto mappedModuleVariable = MappedModuleVariable(v, module);
        if (mappedModuleVariable.getVaHost() != v.getVaHost() || mappedModuleVariable != v) {
            known = std::cref(module->Known(mappedModuleVariable));
        }
        if (known.get().size() == 0) {
            auto name = GetVariableName(v);
            auto moduleName = GetModuleName(name);
            if (!moduleName.empty()) {
                auto module = Module(moduleName);
                name.remove_prefix(moduleName.size() + 1 // '.'
                );
                auto& moduleVarNames = module->GetVarNames();
                auto moduleVarIt = moduleVarNames.find(name);
                if (moduleVarIt == moduleVarNames.end()) {
                    WaitTillModuleLoadingComplete(moduleName);
                    moduleVarIt = moduleVarNames.find(name);
                }
                if (moduleVarIt != moduleVarNames.end()) {
                    auto& moduleVar = moduleVarIt->second;
                    known = std::cref(module->Known(moduleVar));
                } else {
                    auto solution = module->Solve(moduleVarIt->second);
                    if (solution.size()) {
                        known = std::cref(module->Known(moduleVarIt->second));
                        if (known.get().size() == 0) {
                            IMPLEMENT
                        }
                    }
                }
            } else {
                WaitAllModulesLoadingComplete();
                known = std::cref(base::Known(v));
                if (known.get().size() == 0) {
                    Solve(v);
					known = std::cref(base::Known(v));
				}
			}
		}
	}
    return known;
}

// TODO : delete this in favour of moving non-copiable modules to shared modules loader
Skrypt::Skrypt(const skrypt::Skrypt& that)
    : base(that)
    , varHost(that.varHost)
    , vars(that.vars)
    , echo(that.echo)
    , disjunctionParseMode(that.disjunctionParseMode)
    , modules(that.modules)
    , modulesLoading(that.modulesLoading)
    , sourceFilePath(that.sourceFilePath)
    , moduleFileSearchAdditionalPaths(that.moduleFileSearchAdditionalPaths)
{
}