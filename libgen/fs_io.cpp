#include "fs_io.hpp"

#include <boost/gil.hpp>

#include <fstream>


using namespace ::gen::io::fs;


bool ::gen::io::fs::Save(const ::boost::gil::rgba8_image_t& img, const ::boost::filesystem::path& filepath)
{
    (void)img;
    (void)filepath;
    // Image encoding support is intentionally optional in this build.
    // Returning false avoids a hard dependency on Boost.GIL extension-I/O libraries.
    return false;
}
