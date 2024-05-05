#define BOOST_TEST_MODULE School test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"
#include "System.h"


using namespace omnn::math;
using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE(PotatoParadox
	, *disabled() // FIXME:
) {
	std::cout
		<< std::endl
		<< "The potato paradox is a mathematical calculation that has a counter-intuitive result.\n"
			"\"You have 100kg of potatoes, which are 99% water by weight. You let them dehydrate until they're 98% water. How much do they weigh now?\"\n"
			"The surprising answer is 50kg "
		<< std::endl;

	DECL_VA(potatoKg);
    System system;
    system << potatoKg.Equals(100);

	DECL_VA(percentWater);
    system << percentWater.Equals(99);

	DECL_VA(percentWaterDehydrated);
    system << percentWaterDehydrated.Equals(98);

	DECL_VA(potatoPercentDehydrated);
    system << potatoPercentDehydrated.Equals(100 - percentWater);

	DECL_VA(potatoKgDehydrated);
    system << potatoKgDehydrated.Equals(potatoKg * potatoPercentDehydrated / 100);

	DECL_VA(weightWaterDehydrated);
    auto proportion =  (weightWaterDehydrated / percentWaterDehydrated).equals(potatoKgDehydrated / (100 - percentWaterDehydrated));
    system << proportion;

	DECL_VA(newWeight);
    system << newWeight.Equals(potatoKgDehydrated + weightWaterDehydrated);

	auto total = system.Solve(newWeight);
    BOOST_TEST(total.size() == 1);
    auto solution = *total.begin();
    std::cout << solution << std::endl;
    BOOST_TEST(solution == 50);
}

BOOST_AUTO_TEST_CASE(P_474)
{
	std::cout
		<< std::endl
		<< "No 474 Equations"
		<< std::endl;

	DECL_VA(x);
	std::cout << "x+318=645		x=" << (x + 318 - 645)(x) << std::endl;

	DECL_VA(y);
	std::cout << "870-y=187		y=" << (870-y-187)(y) << std::endl;

	DECL_VA(z);
	std::cout << "z-234=356		z=" << (z-234-356)(z) << std::endl;
}


BOOST_AUTO_TEST_CASE(N456)
{
	std::cout
		<< std::endl
		<< "№456"
		<< std::endl;

	DECL_VA(all);
	DECL_VA(soldfirst);
	DECL_VA(soldsecond);
	DECL_VA(free);

	// Усього = ПродалиСпочатку + ПродалиПотім + ВільнихМісць
	// (ПродалиСпочатку + ПродалиПотім + ВільнихМісць) - Усього == 0
	auto formula = (soldfirst + soldsecond + free) - all;
	std::cout << "formula: " << formula.str() << " = 0" << std::endl;

	formula.eval({
		{ all, 586 },
		{ soldfirst, 29 },
		{ soldsecond, 459 }
		});
	std::cout << "evaluated: " << formula.str() << " = 0" << std::endl;

	auto root = formula(free);
	std::cout << "x = " << root << std::endl;

	formula.eval({ { free, root } });
	std::cout << "check: " << formula << " = 0" << std::endl;
	BOOST_TEST(formula == 0);
}

BOOST_AUTO_TEST_CASE(N_451_3)
{
	std::cout
		<< std::endl
		<< "451_3"
		<< std::endl;

	DECL_VA(S);
	DECL_VA(P);
	DECL_VA(x);

	auto formula = (P + x) - S; // S = P+x
	std::cout << "formula: " << formula.str() << " = 0" << std::endl;

	formula.eval({
		{ S, 235 },
		{ P, 127 }
		});
	std::cout << "evaluated: " << formula.str() << " = 0" << std::endl;

	auto root = formula(x);
	std::cout << "x = " << root << std::endl;

	formula.eval({ { x, root } });
	std::cout << "check: " << formula << " = 0" << std::endl;
	BOOST_TEST(formula == 0);
}

BOOST_AUTO_TEST_CASE(P_465_358)
{
	std::cout
		<< std::endl
		<< "Primer: 465-358=" << 586-459
		<< std::endl;
}

BOOST_AUTO_TEST_CASE(Perimeter)
{
    std::cout
        << std::endl
        << "perimeter"
        << std::endl;

    DECL_VA(x);
    DECL_VA(P);
    DECL_VA(a);
    DECL_VA(b);

    auto formula = (a+b)*2 - P;
    std::cout <<"formula: " << formula.str() << " = 0" << std::endl;

    formula.eval({
        { P, 20 },
        { a, x },
        { b, 6 }
    });
    std::cout <<"evaluated: " << formula.str() << " = 0" << std::endl;

    auto root = formula(x);
    std::cout <<"x = " << root << std::endl;

    BOOST_TEST(root == 4);

    formula.eval({ { x, root }});
    std::cout << "check: " << formula << " = 0" << std::endl;
    BOOST_TEST(formula == 0);
}

BOOST_AUTO_TEST_CASE(SquareRootEquationTest) {
    DECL_VA(x);
    auto equation = (x.Sqrt() + 15 + x.Sqrt()) - 15;
    Valuable::solutions_t solutions;
    equation.solve(x, solutions);
    BOOST_TEST(solutions.size() == 1); // Expect one solution
    auto solution = *solutions.begin();
    BOOST_TEST(solution == 0); // The expected solution is x = 0
}
