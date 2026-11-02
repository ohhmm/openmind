#pragma once
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>

namespace gen::io {
namespace fs {

bool Save(const ::boost::gil::rgba8_image_t& img, const ::boost::filesystem::path& filepath);

}}