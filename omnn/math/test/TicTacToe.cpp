#define BOOST_TEST_MODULE TicTacToe test
#include <boost/test/unit_test.hpp>

#include "System.h"

#include <future>
#include <queue>
#include <thread>

using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(TicTacToe_X_Won_test)
{
    System ticTacToe;

    DECL_VA(i);
    ticTacToe << i.Abet({1, 2, 3});

    DECL_VA(v);
    ticTacToe << v.Equals(1); // encoded cross positions

    // The constrasint of X,Y positions to win
    DECL_VA(x);
    DECL_VA(y);
    ticTacToe << ((x.Equals(i) && y.Equals(i)) || (x.Equals(4_v - i) && y.Equals(i)));

	DECL_VA(win);
    auto winExp = (win.Equals(1) && ticTacToe.Total()) || win.Equals(0);

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



    Variant fieldVariant = {};
    for (fieldVariant.code = 1 << 9; fieldVariant.code-->0; ) {

        System fieldExpression;
        fieldExpression << (x.Equals(1) && y.Equals(1) && v.Equals(fieldVariant.pack.hasXat1_1))
                        << (x.Equals(1) && y.Equals(2) && v.Equals(fieldVariant.pack.hasXat1_2))
                        << (x.Equals(1) && y.Equals(3) && v.Equals(fieldVariant.pack.hasXat1_3))
                        << (x.Equals(2) && y.Equals(1) && v.Equals(fieldVariant.pack.hasXat1_1))
                        << (x.Equals(2) && y.Equals(2) && v.Equals(fieldVariant.pack.hasXat1_2))
                        << (x.Equals(2) && y.Equals(3) && v.Equals(fieldVariant.pack.hasXat1_3))
                        << (x.Equals(3) && y.Equals(1) && v.Equals(fieldVariant.pack.hasXat1_1))
                        << (x.Equals(3) && y.Equals(2) && v.Equals(fieldVariant.pack.hasXat1_2))
                        << (x.Equals(3) && y.Equals(3) && v.Equals(fieldVariant.pack.hasXat1_3));
        fieldExpression << winExp;

		auto winSolutions = fieldExpression.Solve(win);
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