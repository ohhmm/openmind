#include <memory>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>
#include <boost/program_options.hpp>
#include <omnn/math/Variable.h>


namespace gen {
	struct options {
		::boost::filesystem::path filepath;
		uint32_t color = 0;
		std::vector<::boost::filesystem::path> blend_files;
	};

    std::shared_ptr<::boost::program_options::options_description> Optr();
	boost::program_options::options_description_easy_init& ProgramOptionsDescription();
	const options& Init(int argc, char** argv); // optional, for options

	const omnn::math::Valuable::va_names_t& InitialVarNames();
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable& n, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable::vars_cont_t& knownValues, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormulaAGRBofWidthHeightXYiN(uint32_t w, uint32_t h, omnn::math::Valuable r, omnn::math::Valuable g, omnn::math::Valuable b);

	bool Generate(const omnn::math::Valuable& pattern, float* data, size_t n);
	bool Generate(const omnn::math::Valuable& pattern, uint32_t* data, size_t n);
	
	boost::gil::rgba8_image_t GeneratePatternImage(uint32_t width, uint32_t height, omnn::math::Valuable red, omnn::math::Valuable green, omnn::math::Valuable blue);
}
