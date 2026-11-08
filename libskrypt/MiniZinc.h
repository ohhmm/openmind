#pragma once

#include <boost/filesystem.hpp>
#include <memory>

namespace omnn { namespace math { class System; } }

namespace skrypt {

class MiniZincLoader {
public:
    static std::shared_ptr<omnn::math::System> Load(const boost::filesystem::path& modelPath);
};

} // namespace skrypt
