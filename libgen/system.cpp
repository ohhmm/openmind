#include "system.hpp"

#include <boost/cast.hpp>

#include <iostream>


using namespace gen;


void PatternRelationSystem::Init()
{
    Add(w, width);
    Add(h, height);
    Add(r, red);
    Add(g, green);
    Add(b, blue);
    Add(a, alpha);
	//Add(color.Equals((((b % 256) + alpha.Shl(8)).shl(8) + (g % 256)).shl(8) + (r % 256)));
}

PatternRelationSystem::PatternRelationSystem()
    : base()
{
    Init();
}

PatternRelationSystem::PatternRelationSystem(std::istream& in)
: base()
{
    Init();
	Load(in);
}

bool PatternRelationSystem::Ask(const omnn::math::Variable& v)
{
	std::cout << "\nWhat would be the " << v << '?' << std::endl;
	bool result = {};
	std::string line;
	do {
		std::cin >> line;
		if(!line.empty()) {
			Add(v.Equals({ line, GetVarHost() }));
			result = true;
		} else if (std::cin.eof()) {
			break;
		} else {
			std::cout << "Any other clues?" << std::endl;
			std::cin >> line;
			Add({ line, GetVarHost() });
		}
	} while (line.empty() && !std::cin.eof());
	return result;
}

omnn::math::Valuable PatternRelationSystem::Get(const omnn::math::Variable& v)
{
	do
	{
		auto known = Known(v);
		if (known.size()) {
			return *known.cbegin();
		}

		try {
			solutions_t solutions;
			solutions = Solve(v);
			auto amount = solutions.size();
			if (amount == 1) {
				return std::move(solutions.extract(solutions.cbegin()).value());
			}
			else {
				std::cout << "Disambiguation for " << v << ": [";
				for (auto& item : solutions)
				{
					std::cout << item << ' ';
				}
				std::cout << ']';
			}
		}
		catch (...)
		{
		}
	} while (Ask(v));

	Fetch(v);
	solutions_t solutions;
	solutions = Solve(v);
	return std::move(solutions.extract(solutions.cbegin()).value());
}

boost::gil::rgba8_image_t PatternRelationSystem::GeneratePatternImage()
{
	auto heightValue = boost::numeric_cast<size_t>(Get(height).ca());
	auto widthValue = boost::numeric_cast<size_t>(Get(width).ca());

    boost::gil::rgba8_image_t img(widthValue, heightValue);
	auto view = boost::gil::view(img);
    auto variables = Vars();
    for (auto row=heightValue; row-->0 ;)
	{
        auto copyForRow = *this;
        copyForRow.Eval(y, row);

        for (auto col=widthValue; col-->0 ;)
        {
			struct PixelCache {
				omnn::math::System::v_es_ptr_t yarns = std::make_shared<omnn::math::System::v_es_t>();
				~PixelCache() { yarns->clear(); }
			} cache;
			for (const auto& variable : variables) {
				auto& yarns = (*cache.yarns)[variable];
				yarns = std::make_shared<omnn::math::System::es_t>();
				for (const auto& [parameters, expressions] : Yarns(variable)) {
					omnn::math::System subsystem(cache.yarns);
					subsystem.insert(expressions.begin(), expressions.end());
					yarns->emplace(parameters, std::move(subsystem));
				}
			}
            auto copy = copyForRow;
			auto& pixelSystem = static_cast<omnn::math::System&>(copy);
			pixelSystem = omnn::math::System(cache.yarns);
			pixelSystem.MakesTotalEqu(MakesTotalEqu());
			if (!MakesTotalEqu())
				pixelSystem.insert(begin(), end());
			else for (const auto& expression : Expressions())
				pixelSystem.Add(expression);
			copy.Eval(x, col);

			using namespace boost::gil;
			auto& px = view(col, row);
			get_color(px, alpha_t()) = boost::numeric_cast<uint8_t>(copy.Get(alpha).ca());;
			get_color(px, blue_t()) = boost::numeric_cast<uint8_t>(copy.Get(blue).ca());;
			get_color(px, green_t()) = boost::numeric_cast<uint8_t>(copy.Get(green).ca());;
			get_color(px, red_t()) = boost::numeric_cast<uint8_t>(copy.Get(red).ca());;
		}
    }
    return img;
}
