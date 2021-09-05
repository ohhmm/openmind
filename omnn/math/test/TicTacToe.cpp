#define BOOST_TEST_MODULE TicTacToe test
#include <boost/test/unit_test.hpp>

#include "System.h"
#include "VarHost.h"

#include <future>
#include <queue>
#include <thread>

using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;



namespace {
	struct EncodedFieldPack { // of encoded field (X only)
		bool hasXat1_1 : 1;   // bit
		bool hasXat1_2 : 1;   // bit
		bool hasXat1_3 : 1;   // bit
		bool hasXat2_1 : 1;   // bit
		bool hasXat2_2 : 1;   // bit
		bool hasXat2_3 : 1;   // bit
		bool hasXat3_1 : 1;   // bit
		bool hasXat3_2 : 1;   // bit
		bool hasXat3_3 : 1;   // bit
	};
	// Enumerating all variants and dump matched variants:
	union Variant {
		int code : 9; // bits
		EncodedFieldPack pack;
	};

	DECL_VA(i);
	DECL_VA(v);
	// The constrasint of X,Y positions to win
	DECL_VA(x);	DECL_VA(dx);
	DECL_VA(y);	DECL_VA(dy);
	DECL_VA(win);

	// precalculated & optimized:
    std::string_view precalculatedOptimizedIsWon = "((dy^12)*win + (dx^12)*win + -8*(dy^11)*win + -8*(dx^11)*win + 6*(dy^10)*(dx^2)*win + 6*(dx^10)*(dy^2)*win + -8*(dy^10)*win*dx + -8*(dx^10)*win*dy + 32*(dy^10)*win + 32*(dx^10)*win + -40*(dy^9)*(dx^2)*win + -40*(dx^9)*(dy^2)*win + 56*(dy^9)*win*dx + 56*(dx^9)*win*dy + -84*(dy^9)*win + -84*(dx^9)*win + 15*(dy^8)*(dx^4)*win + 15*(dx^8)*(dy^4)*win + -40*(dy^8)*(dx^3)*win + -40*(dx^8)*(dy^3)*win + 160*(dy^8)*(dx^2)*win + 160*(dx^8)*(dy^2)*win + -196*(dy^8)*win*dx + -196*(dx^8)*win*dy + 162*(dy^8)*win + 162*(dx^8)*win + -80*(dy^7)*(dx^4)*win + -80*(dx^7)*(dy^4)*win + 224*(dy^7)*(dx^3)*win + 224*(dx^7)*(dy^3)*win + -448*(dy^7)*(dx^2)*win + -448*(dx^7)*(dy^2)*win + 448*(dy^7)*win*dx + 448*(dx^7)*win*dy + -244*(dy^7)*win + -244*(dx^7)*win + 20*(dy^6)*(dx^6)*win + -80*(dy^6)*(dx^5)*win + -80*(dx^6)*(dy^5)*win + 320*(dy^6)*(dx^4)*win + 320*(dx^6)*(dy^4)*win + -672*(dy^6)*(dx^3)*win + -672*(dx^6)*(dy^3)*win + 920*(dy^6)*(dx^2)*win + 920*(dx^6)*(dy^2)*win + -740*(dy^6)*win*dx + -740*(dx^6)*win*dy + 296*(dy^6)*win + 296*(dx^6)*win + (i^6)*win + 336*(dy^5)*(dx^5)*win + -840*(dy^5)*(dx^4)*win + -840*(dx^5)*(dy^4)*win + 1344*(dy^5)*(dx^3)*win + 1344*(dx^5)*(dy^3)*win + -1420*(dy^5)*(dx^2)*win + -1420*(dx^5)*(dy^2)*win + 928*(dy^5)*win*dx + 928*(dx^5)*win*dy + -12*(i^5)*win + -292*(dy^5)*win + -292*(dx^5)*win + 1516*(dy^4)*(dx^4)*win + -1916*(dy^4)*(dx^3)*win + -1916*(dx^4)*(dy^3)*win + 1656*(dy^4)*(dx^2)*win + 1656*(dx^4)*(dy^2)*win + -900*(dy^4)*win*dx + -900*(dx^4)*win*dy + 233*(dy^4)*win + 233*(dx^4)*win + 58*(i^4)*win + 1984*(dy^3)*(dx^3)*win + -1448*(dy^3)*(dx^2)*win + -1448*(dx^3)*(dy^2)*win + 672*(dy^3)*win*dx + 672*(dx^3)*win*dy + -144*(i^3)*win + -148*(dy^3)*win + -148*(dx^3)*win + (win^3) + 914*(dy^2)*(dx^2)*win + (i^2)*(dy^2)*win + (i^2)*(dx^2)*win + -372*(dy^2)*win*dx + -372*(dx^2)*win*dy + 193*(i^2)*win + 72*(dy^2)*win + 72*(dx^2)*win + (y^2)*win + (x^2)*win + (v^2)*win + -2*(win^2) + -2*y*win*i*dy + -2*x*win*i*dx + 136*win*dy*dx + -2*win*v + -24*win*dy + -24*win*dx + -132*win*i + 42*win)";
    /* auto isWon = // Source code of the expression:
                            (( (dx.Equals(0) && dy.Equals(1))
                            || (dx.Equals(1) && dy.Equals(0))
                            || (dx.Equals(1) && dy.Equals(1))
                             ).Sq()
                            + i.Abet({1, 2, 3}).Sq()
                            + x.Equals(i * dx).Sq()
                            + y.Equals(i * dy).Sq()
                            + v.Equals(1).Sq()
                            + win.Equals(1).Sq()
                            ) || win.Equals(0);
                            */
    Valuable::va_names_t variables{{"i", i}, {"v", v}, {"x", x}, {"y", y}, {"dx", dx}, {"dy", dy}, {"win", win}};
    Valuable isWon(precalculatedOptimizedIsWon, variables, true);
} // namespace
BOOST_AUTO_TEST_CASE(TicTacToe_X_Won_test)
{
	//auto iPossibleValues = i.Abet({1, 2, 3});
                     
    auto isWonSq = isWon;
	isWonSq.sq();
		
	//System ticTacToe;

    //ticTacToe << iPossibleValues;

    //ticTacToe << v.Equals(1); // encoded cross positions
    
    //ticTacToe << ((x.Equals(i) && y.Equals(i)) || (x.Equals(4_v - i) && y.Equals(i)));

	//auto winExp = (win.Equals(1) && ticTacToe.Total()) || win.Equals(0);
    auto xIs1 = x.Equals(1);	auto xIs1sq = xIs1.Sq();
    auto xIs2 = x.Equals(2);    auto xIs2sq = xIs2.Sq();
    auto xIs3 = x.Equals(3);    auto xIs3sq = xIs3.Sq();
    auto yIs1 = y.Equals(1);    auto yIs1sq = yIs1.Sq();
    auto yIs2 = y.Equals(2);    auto yIs2sq = yIs2.Sq();
    auto yIs3 = y.Equals(3);    auto yIs3sq = yIs3.Sq();
    Variant fieldVariant = {};
    for (fieldVariant.code = -1; fieldVariant.code--;) {
        std::cout << "checking\n    " << (fieldVariant.pack.hasXat1_1 ? 'X' : '0')
                  << (fieldVariant.pack.hasXat1_2 ? 'X' : '0') << (fieldVariant.pack.hasXat1_3 ? 'X' : '0') << std::endl
                  << "    " << (fieldVariant.pack.hasXat2_1 ? 'X' : '0') << (fieldVariant.pack.hasXat2_2 ? 'X' : '0')
                  << (fieldVariant.pack.hasXat2_3 ? 'X' : '0') << std::endl
                  << "    " << (fieldVariant.pack.hasXat3_1 ? 'X' : '0') << (fieldVariant.pack.hasXat3_2 ? 'X' : '0')
                  << (fieldVariant.pack.hasXat3_3 ? 'X' : '0') << std::endl;

        auto fieldExpression = Product{xIs1sq + yIs1sq + v.Equals(fieldVariant.pack.hasXat1_1).Sq(),
                                       xIs1sq + yIs2sq + v.Equals(fieldVariant.pack.hasXat1_2).Sq(),
                                       xIs1sq + yIs3sq + v.Equals(fieldVariant.pack.hasXat1_3).Sq(),
                                       xIs2sq + yIs1sq + v.Equals(fieldVariant.pack.hasXat1_1).Sq(),
                                       xIs2sq + yIs2sq + v.Equals(fieldVariant.pack.hasXat1_2).Sq(),
                                       xIs2sq + yIs3sq + v.Equals(fieldVariant.pack.hasXat1_3).Sq(),
                                       xIs3sq + yIs1sq + v.Equals(fieldVariant.pack.hasXat1_1).Sq(),
                                       xIs3sq + yIs2sq + v.Equals(fieldVariant.pack.hasXat1_2).Sq(),
                                       xIs3sq + yIs3sq + v.Equals(fieldVariant.pack.hasXat1_3).Sq()};
        fieldExpression.sq();
        fieldExpression += isWonSq;

        fieldExpression.optimize();
		auto winSolutions = fieldExpression.GetIntegerSolution(win);
        if (winSolutions.size() == 1) {
            auto it = winSolutions.begin();
            if (*it == 1) {
                std::cout
					<< "Win:" << (fieldVariant.pack.hasXat1_1 ? 'X' : '0') << (fieldVariant.pack.hasXat1_2 ? 'X' : '0') << (fieldVariant.pack.hasXat1_3 ? 'X' : '0') << std::endl
					<< "    " << (fieldVariant.pack.hasXat2_1 ? 'X' : '0') << (fieldVariant.pack.hasXat2_2 ? 'X' : '0') << (fieldVariant.pack.hasXat2_3 ? 'X' : '0') << std::endl
					<< "    " << (fieldVariant.pack.hasXat3_1 ? 'X' : '0') << (fieldVariant.pack.hasXat3_2 ? 'X' : '0') << (fieldVariant.pack.hasXat3_3 ? 'X' : '0') << std::endl;
            }
        } else {
            for (auto& s : winSolutions) {
                std::cout << s << std::endl;
			}
		}
	}
}