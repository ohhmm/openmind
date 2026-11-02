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

void PatternRelationSystem::Ask(const omnn::math::Variable& v)
{
	std::cout << "\nWhat would be the " << v << '?' << std::endl;
	std::string line;
	do {
	std::cin >> line;
	if(!line.empty())
		Add(v.Equals({ line, GetVarHost() }));
	else {
		std::cout << "Any other clues?" << std::endl;
		std::cin >> line;
		Add({ line, GetVarHost() });
	}
	} while (line.empty());
}

omnn::math::Valuable PatternRelationSystem::Get(const omnn::math::Variable& v)
{
	for (;;)
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

		Ask(v);
	}
}

boost::gil::rgba8_image_t PatternRelationSystem::GeneratePatternImage()
{
	auto heightValue = boost::numeric_cast<size_t>(Get(height).ca());
	auto widthValue = boost::numeric_cast<size_t>(Get(width).ca());

    boost::gil::rgba8_image_t img(widthValue, heightValue);
	auto view = boost::gil::view(img);
    for (auto row=heightValue; row-->0 ;)
	{
        for (auto col=widthValue; col-->0 ;)
        {
			auto copy = *this;
			copy.Eval(x, col);
			copy.Eval(y, row);

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
