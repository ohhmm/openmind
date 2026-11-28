#include "fs_io.hpp"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png/write.hpp>

#include <fstream>


using namespace ::gen::io::fs;


bool ::gen::io::fs::Save(const ::boost::gil::rgba8_image_t& img, const ::boost::filesystem::path& filepath)
{
    try {
        auto& mutableImg = const_cast<::boost::gil::rgba8_image_t&>(img);
        boost::gil::write_view(filepath.string(), boost::gil::view(mutableImg), boost::gil::png_tag());
        return true;
    } catch (...) {
        return false;
    }
}
