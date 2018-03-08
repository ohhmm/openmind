#define BOOST_TEST_MODULE Sum test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"

using namespace omnn::math;

std::string l(const Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

void ohashes(const Valuable& v)
{
    auto s = Sum::cast(v);
    for(auto& i : *s)
    {
        std::cout << i << " hash "<< i.Hash()<<std::endl;
    }
}

BOOST_AUTO_TEST_CASE(Sum_tests)
{
    Variable v1,v2,v3;
    
    auto _1 = (v1 + -1)*v1;
    auto _2 = (v1^2) + -1*v1;
    BOOST_TEST(_1 == _2);
    
    _1 = Sum{1, -1};
    BOOST_TEST(_1 == 0_v);
    
    auto val1 = -2_v/3*v1;
    val1 += v1*v1;
    val1 += 1_v/9;
    auto val2 = 1_v/9 + v1*v1 + -2_v/3*v1;
    std::cout << val1 << " == " << val2 << std::endl;
    BOOST_TEST((-2_v/3*v1 + v1*v1 + 1_v/9) == (1_v/9 + v1*v1 + -2_v/3*v1));
    BOOST_TEST(1_v/9 - (2_v/3) * v3 + v3*v3 == 1_v/9 - (2_v/3) * v3 + v3*v3);
    
    auto t = v3-1;
    BOOST_TEST(((v3-1)^2) == t^2);
    auto sq = t*t;
    sq.optimize();
    BOOST_TEST(sq == t^2);
    BOOST_TEST(sq == (v3^2)-v3*2+1);
    _1 = sq/t;
    BOOST_TEST(_1 == t);
    
    Valuable
        f = 1_v / 2,
        f1 = 5_v / 6,
        a = 1_v + 2 + f + 3,
        b = 6_v + f1;
    
    std::cout << a << std::endl;
    a -= b;
    std::cout << a << std::endl;
    
    auto s=a;
    s+=v3;
    std::cout << s;
    BOOST_TEST(s == a+v3);
    std::cout << s << std::endl;
    ohashes(s);
    ohashes(-1_v/3 + v1);
    std::cout << (-2_v/3 * v1).Hash() << std::endl;
    s = s*s;
    std::cout << s << std::endl;
    ohashes(s);
    s.optimize();
    ohashes(s);
    t = 1_v/9 - (2_v/3) * v3 + v3*v3;
    ohashes(t);
    BOOST_TEST(s == 1_v/9 - (2_v/3) * v3 + v3*v3);

    BOOST_TEST((v1*v2 + v1*v1*v2 + 1/3*125*v1 + 1/3*125*v1*v1) == (v1*v2 + 1/3*125*v1 + 1/3*125*v1*v1 + v1*v1*v2));
//    BOOST_TEST((((65851823091255177969664_v*v11*v11*v11*v11*v11 + -4433312658227724288*v11*v11*v11*v11*v11*v11))/(v11*v11*v11*v11*v11*v11*v11*v11*v11*v11*v11) + v9*v9*-23702740992/(v11*v11*v11*v11*v11) + v10*v10*-23702740992/(v11*v11*v11*v11*v11)) == (((65851823091255177969664*v11*v11*v11*v11*v11 + -4433312658227724288*v11*v11*v11*v11*v11*v11))/(v11*v11*v11*v11*v11*v11*v11*v11*v11*v11*v11) + v9*v9*-23702740992/(v11*v11*v11*v11*v11) + v10*v10*-23702740992/(v11*v11*v11*v11*v11)))
    s = (v3+1)+(v1+v2);
    s.optimize();
    auto sc = Sum::cast(s);
    BOOST_TEST(sc);
    BOOST_TEST(sc->size()==4);
    BOOST_TEST((((8_v + (((8*1 + 8*1 + -8 + (1_v^2)*-4 + (1_v^2)*-4))^((1_v/2)))))/2) == 4);
    
    Variable x,y,z;
    _1 = (x+y)*(x-y);
    _2 = (x^2) - (y^2);
    BOOST_TEST(_1 == _2);
    sq = (x^4) + (z^4);
    auto sum = Sum::cast(sq);
    BOOST_TEST(sum);
    if (sum) {
        BOOST_TEST(sum->size()==2);
    }
    t=-2*y-2*x;
    sum=Sum::cast(t);
    BOOST_TEST(sum);
    if (sum) {
        BOOST_TEST(sum->size()==2);
    }
    sq = (x^4) + (z^4) + (y^4) - 4*x*(z^2) - 4 *(x^2)* y - 4*x*(y^2) - 4*y*(z^2) - 16*(x^2)*z - 16*(y^2)*z - 72*y - 72*x + 40*(x^2) + 40*(y^2) + 8*x*y + 32*x*z + 32*y*z - 4*(y^3) - 4*(x^3) + 2*(x^2)*(y^2) + 2*(y^2)*(z^2) + 2*(x^2)*(z^2)    - 16*(z^3) + 100*(z^2) - 288*z + 324;
    sum = Sum::cast(sq);
    BOOST_TEST(sum);
    BOOST_TEST(sum->size()==25);
    auto sqc = sq;
    sqc.optimize();
    BOOST_TEST(sqc == sq);
    
    auto _ = (z^2) -2*y + (y^2) -2*x + (x^2) -8*z + 18;
    sum = Sum::cast(_);
    BOOST_TEST(sum);
    BOOST_TEST(sum->size()==7);
    t = _;
    t.Eval(x,1);
    t.Eval(y,1);
    t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t==0);
    
    t = sqc;
    t.Eval(x,1); t.Eval(y,1); t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t==0);
    
    Variable v4,v5,v6;
    // shuffled compare test
    BOOST_TEST((v5^2)*-4*v4 == (v5^2)*-4*v4);
    BOOST_TEST(v4+v5 == v5+v4);
    BOOST_TEST(v4*v6+v5*v6 == v5*v6+v4*v6);
    
    auto t1 = (v4^2)*v5*-4 + (v5^2)*-4*v4;
    auto t2 = (v5^2)*-4*v4 + (v4^2)*v5*-4;
    auto s1 = Sum::cast(t1);
    auto s2 = Sum::cast(t2);
    BOOST_TEST(s1->size()==s2->size());
    BOOST_TEST(s1->size()==2);
//    BOOST_TEST(*s1->begin()==*s2->GetConstCont().rbegin());
//    BOOST_TEST(*s2->begin()==*s1->GetConstCont().rbegin());
    BOOST_TEST(t1 == t2);
    
    t1 = ((v4^4) + (v6^4) + (v5^4) + (v6^2)*-4*v4 + (v4^2)*v5*-4 + (v5^2)*-4*v4 + (v6^2)*v5*-4 + (v4^2)*-16*v6 + (v5^2)*-16*v6 + -72*v5 + -72*v4 + -288*v6 + 324 + (v6^2)*100 + (v4^2)*40 + (v5^2)*40 + 8*v5*v4 + (v6^3)*-16 + 32*v6*v4 + 32*v5*v6 + (v5^3)*-4 + (v4^3)*-4 + (v6^2)*(v5^2)*2 + (v5^2)*(v4^2)*2 + (v6^2)*(v4^2)*2);
    t2 = ((v4^4) + (v6^4) + (v5^4) + (v6^2)*-4*v4 + (v5^2)*-4*v4 + (v4^2)*v5*-4 + (v6^2)*v5*-4 + (v4^2)*-16*v6 + (v5^2)*-16*v6 + -72*v5 + -72*v4 + -288*v6 + 324 + (v6^2)*100 + (v4^2)*40 + (v5^2)*40 + 8*v5*v4 + (v6^3)*-16 + 32*v6*v4 + 32*v5*v6 + (v5^3)*-4 + (v4^3)*-4 + (v6^2)*(v5^2)*2 + (v5^2)*(v4^2)*2 + (v6^2)*(v4^2)*2);
    BOOST_TEST(t1 == t2);
    t = _*_;
    BOOST_TEST(t==sqc);
    t.Eval(x,1); t.Eval(y,1); t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t==0);
    
    _ = ((v3 + (2040_v/v1)) / ((-1_v/v1)*v2));
    auto ss = _(v3);
    BOOST_TEST(ss.size() == 1);
    BOOST_TEST(!ss.begin()->HasVa(v3));
    _ = *ss.begin();
    BOOST_TEST(_ == -2040_v/v1);
    
    _1 = x*-1+y*-2;
    _1 /= z;
    BOOST_TEST(_1.HasVa(z));
    _2 = x*-1/z+y*-2/z;
    BOOST_TEST(_1==_2);
    
    Variable v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17;
    
    _1 = -1_v*v17*v12 + -1_v*v16*v13 + 174;
    _2 = -1_v*v17*v12 + -1_v*v13*v16 + 174;
    BOOST_TEST(_1 == _2);

    _1 = -1_v*(v11^2)*v16 + -1_v*v17*v10*v11 + -1_v*v15*v12*v11 + -1_v*v14*v13*v11 + 2723_v*v11;
    _2 = -1_v*(v11^2)*v16 + -1_v*v13*v11*v14 + -1_v*v17*v10*v11 + -1_v*v15*v12*v11 + 2723_v*v11;
    BOOST_TEST(_1 == _2);

    _ = (-1_v*(v6^-1_v)*(v8^2_v)*v13 + -1_v*(v6^-1_v)*v14*v7*v8 + -1_v*(v6^-1)*v12*v9*v8 + v10*v13 + v9*v14 + -1_v*(v6^-1)*v11*v10*v8 + 2723_v*(v6^-1)*v8 + -174_v);
    // -1_v*(v6^-1_v)*(v8^2_v)*v13 + -1_v*(v6^-1_v)*v14*v7*v8 + -1_v*(v6^-1)*v12*v9*v8 -1_v*(v6^-1)*v11*v10*v8 + 2723_v*(v6^-1)*v8
    //              == -v10*v13 - v9*v14 + 174_v
    
    // -1_v*(v8^2_v)*v13 + -1_v*v14*v7*v8 + -1_v*v12*v9*v8 -1_v*v11*v10*v8 + 2723_v*v8 == v6(-v10*v13 - v9*v14 + 174_v)
    // v6 == (-1_v*(v8^2_v)*v13 + -1_v*v14*v7*v8 + -1_v*v12*v9*v8 -1_v*v11*v10*v8 + 2723_v*v8) / (-v10*v13 - v9*v14 + 174_v)
    ss = _(v6);
    BOOST_TEST(ss.size() == 1);
    BOOST_TEST(!ss.begin()->HasVa(v6));
    _1 = *ss.begin();
    _2 = Fraction(-1_v*(v8^2_v)*v13 + -1_v*v14*v7*v8 + -1_v*v12*v9*v8 -1_v*v11*v10*v8 + 2723_v*v8,
                  -v10*v13 - v9*v14 + 174_v);
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Become_tests)
{
    Variable v;
    Valuable f = 1_v/2;
    auto s = f+v;
    
    BOOST_TEST(reinterpret_cast<Valuable*>(&s)->exp);
    bool isValuableType = typeid(s)==typeid(Valuable);
    BOOST_TEST(isValuableType);
    bool isInnerSum = typeid(*s.exp.get())==typeid(Sum);
    BOOST_TEST(isInnerSum);
}

BOOST_AUTO_TEST_CASE(Solution_tests)
{
    Variable v;
    auto sol = ((v^2)-4_v).solutions(v);
    BOOST_TEST(sol.size()==2);
    auto solution1 = *sol.begin();
    BOOST_TEST(solution1.abs() == 2_v);
    auto solution2 = *++sol.begin();
    BOOST_TEST(solution1 == -solution2);
}

BOOST_AUTO_TEST_CASE(Containers_test)
{
    Variable va;
    std::vector<Valuable> vec{0_v, va, va+1_v};
    std::set<Valuable, SumOrderComparator> s;
    std::unordered_set<Valuable> us;
    for(auto& a : vec)
    {
        s.insert(a);
        us.insert(a);
    }
    BOOST_TEST(vec.size()==3);
    BOOST_TEST(s.size()==3);
    BOOST_TEST(us.size()==3);
}

BOOST_AUTO_TEST_CASE(PolynomialDivHang_test_no_hang, *timeout(2))
{
    // good
    auto _1 = (va^2)-va*2+1;
    auto _2 = va-1;
    BOOST_TEST(_1/_2 == _2);
    
    // hang -> should fall back to fraction
    _2 = (va^2)-1;
    auto _ = _1/_2; // hang
    BOOST_TEST(_.IsFraction());
    BOOST_TEST(_1/_2 == (((va^2)-va*2+1)/((va^2)-1)));
}
