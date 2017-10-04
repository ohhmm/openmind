#define BOOST_TEST_MODULE Product test
#include <boost/test/unit_test.hpp>

#include "Product.h"
#include "Sum.h"
#include "Fraction.h"
#include "Variable.h"

BOOST_AUTO_TEST_CASE(Product_tests)
{
	using namespace std;
	using namespace omnn::extrapolator;
	using s_t = Sum;
	using f_t = Fraction;
	//Variable v;
	f_t f(1 , 2);
	f_t f1(5, 6);
	Product p(f, f1,5,20);
	/*s_t a(1,2, f, 3) ;
	s_t b(6, f1);
	a -= b;
	a += Variable();*/
	p.optimize();
	cout << p << endl;
	/*s_t s(f1, 5);
	p *=s;
	std::cout << p;*/
}
