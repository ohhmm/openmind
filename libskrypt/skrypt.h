#pragma once
#include <omnn/rt/tasq.h>
#include <omnn/math/System.h>
#include <omnn/math/VarHost.h>

#include <boost/typeof/typeof.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/log/sinks.hpp>

#include <ostream>
#include <map>
#include <shared_mutex>
#include <string_view>


namespace skrypt {

class Skrypt
    : public omnn::math::System
{
    using base = ::omnn::math::System;
    using path = ::boost::filesystem::path;

    ::omnn::math::VarHost::ptr varHost = ::omnn::math::VarHost::make<std::string>();

    ::omnn::math::Valuable::va_names_t vars;
    bool echo = {};
    bool disjunctionParseMode = {};

    using module_t = std::shared_ptr<Skrypt>;
    using modules_t = std::map<std::string_view, module_t>;
    using loading_module_t = std::shared_future<module_t>;
    using loading_modules_t = std::map<std::string_view, loading_module_t>;
    using loading_modules_future_t = std::future<loading_modules_t>;

    modules_t modules;
    std::shared_mutex modulesMapMutex;
    std::shared_mutex modulesLoadingMutex;
    modules_t modulesLoading;
    ::omnn::rt::StoringTasksQueue<loading_modules_t> modulesLoadingQueue;
    path sourceFilePath;
    std::vector<path> moduleFileSearchAdditionalPaths;
    std::vector<std::shared_ptr<std::ostream>> outputs;

protected:
    void SetVarhost(decltype(varHost));
    constexpr bool DisjunctionParseMode() const { return disjunctionParseMode; }
    void DisjunctionParseMode(bool isDisjunctionMode) { disjunctionParseMode = isDisjunctionMode; }
    ::omnn::math::Product disjunction;
    bool ParseTotal(std::istream&, std::string_view&, std::function<void(::omnn::math::Valuable&&)>);

public:
    using value_type = omnn::math::Valuable;

    Skrypt() {}
    Skrypt(const Skrypt&);
    Skrypt(const boost::filesystem::path&);
    Skrypt(std::istream&);

    auto GetVarHost() const { return varHost; }
    const auto& GetVarNames() const { return vars; }

    using base::Add;

    bool Add(::omnn::math::Valuable&&);
    bool Add(std::string_view);
    bool ParseNextLine(std::istream&, std::string_view&);
    void PrintVarKnowns(const omnn::math::Variable&);
    void PrintAllKnowns();
    void ProcessQuestionLine(std::string_view&);

    void BindTargetStream(std::ostream&);
    void BindTargetStream(std::shared_ptr<std::ostream>);
    void BindTargetStream(const boost::filesystem::path&);

    /// <summary>
    /// Loads .skrypt file and returns InitialVarNames for input
    /// </summary>
    /// <returns></returns>
    const omnn::math::Valuable::va_names_t& Load(std::istream&);

    /// <summary>
    /// Loads .skrypt file and returns InitialVarNames for input
    /// </summary>
    /// <returns></returns>
    const omnn::math::Valuable::va_names_t& Load(const boost::filesystem::path&);

    /// <summary>
    /// Loads or gets loaded .skrypt file and returns Skrypt object reference
    /// </summary>
    /// <returns>Skrypt&</returns>
    /// <param name="name">The module name</param>
    module_t Module(std::string_view name);
    module_t Module(const ::omnn::math::Variable&);
    module_t GetLoadedModule(std::string_view name);

    boost::filesystem::path FindModulePath(std::string_view name) const;
    template <class T>
    void AddModuleSearchDirPath(T&& p) {
		moduleFileSearchAdditionalPaths.emplace_back(std::forward<T>(p));
    }
    bool IsModuleLoading(std::string_view name);

    loading_module_t StartLoadingModule(std::string_view name);
    loading_modules_t LoadModules(const ::omnn::math::Valuable& v);
    loading_modules_future_t StartLoadingModules(const ::omnn::math::Valuable& v);
    void BackgroundLoadingModules(const ::omnn::math::Valuable& v);

    /// <summary>
    /// Wait for the module to be loaded
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    module_t WaitTillModuleLoadingComplete(std::string_view name);
    void WaitAllModulesLoadingComplete();

    std::string_view GetVariableName(const ::omnn::math::Variable&) const;
    std::string_view GetModuleName(std::string_view variableName) const;
    std::string_view GetModuleName(const ::omnn::math::Variable&) const;
    const solutions_t& Known(const ::omnn::math::Variable& v);

    /// <summary>
    /// Obtainse the <variable> from the <module> by its local representation variable <module>.<variable>
    /// If module object providen, the variable is obtained from it, otherwise the module deduced from the variable name
    /// </summary>
    /// <param name="variable">The variable</param>
    /// <param name="module">The module (optional)</param>
    /// <returns>Variable of submodule skrypt object</returns>
    const ::omnn::math::Variable& MappedModuleVariable(const ::omnn::math::Variable&, module_t module = {});

    void Echo(bool e) { echo = e; }
};

} // namespace skrypt
