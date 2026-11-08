#include "MiniZinc.h"

#include "skrypt.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <stdexcept>

namespace skrypt {

std::shared_ptr<omnn::math::System> MiniZincLoader::Load(const boost::filesystem::path& modelPath) {
    if (!boost::filesystem::exists(modelPath))
        throw std::runtime_error("MiniZinc model not found: " + modelPath.string());

    boost::filesystem::ifstream stream(modelPath);
    if (!stream)
        throw std::runtime_error("MiniZinc model could not be opened: " + modelPath.string());

    (void)stream;
    return std::make_shared<Skrypt>();
}

} // namespace skrypt
