//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE System test
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_USES_MOVE
#include <boost/test/unit_test.hpp>

#include "System.h"
#include "VarHost.h"
#include "Euler.h"
#include "i.h"
#include "pi.h"

#include <array>
#include <chrono>
#include <future>
#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread/executor.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/tokenizer.hpp>


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(Basic_System_tests) {
    auto& a = "a"_va;
    System s;
    s << a + (a ^ 2);
    {
        DECL_VARS(b, c);
        System sys;
        auto e1 = a.Abet({1, 2});
        auto e2 = a.Abet({2, 3});
        auto both = e1 && e2;

        auto solExpInt = both.IntSolutions();
        auto solExp = both.Solutions();
        BOOST_TEST(solExpInt == solExp);

        auto expHasOneSolution = solExp.size() == 1;
        BOOST_TEST(expHasOneSolution);

        sys << e1 << e2;

        auto sysTotal = sys.MakesTotalEqu() ? sys.Total() : sys.CalculateTotalExpression();
        // FIXME: BOOST_TEST(sysTotal == both);

        auto solSys = sys.Solve(a);
        auto solSysIt = solSys.find(*solExp.begin());
        auto sysHasTheSolution = solSysIt != solSys.end();
        BOOST_TEST(sysHasTheSolution);
        if (sysHasTheSolution) {
            auto _ = *solSysIt;
            BOOST_TEST(_ == 2);
        }
    }
}

BOOST_AUTO_TEST_CASE(Multivariable_System_tests
    , *disabled() // FIXME:
) {
        DECL_VARS(x, y, z);
        System sys;
        sys << x - 3*y + 3*z + 4;
        sys << 2*x + 3*y - z - 15;
        sys << 4*x - 3*y - z - 19;
        auto sol = sys.Solve(x);
        BOOST_TEST(sol.size() == 1);
        if (sol.size()) {
            auto _ = *sol.begin();
            BOOST_TEST(_ == 5);
        }
        sol = sys.Solve(y);
        BOOST_TEST(sol.size() == 1);
        if (sol.size()) {
            auto _ = *sol.begin();
            BOOST_TEST(_ == 1);
        }
        sol = sys.Solve(z);
        BOOST_TEST(sol.size() == 1);
        if (sol.size()) {
            auto _ = *sol.begin();
            BOOST_TEST(_ == -2);
        }

}

BOOST_AUTO_TEST_CASE(NonLinear_System_tests
    , *disabled() // FIXME:
) {
    {
        DECL_VARS(x, y, z);
        System sys;
        sys << "(x^2)+(y^2)=8"_v
            << "x*y=7"_v
            << "(x+y)^2=z"_v;
        auto _ = sys.Solve(z);
    }
    {
        DECL_VARS(a, b, c);
        System sys;
        sys << a + b + c - 6;
        sys << (a ^ 2) + (b ^ 2) + (c ^ 2) - 14;
        sys << (a ^ 3) + (b ^ 3) + (c ^ 3) - 36;
        auto _ = sys.Solve(a);
        BOOST_TEST(_.size() == 1);
    }
    {
        DECL_VARS(a, b, c);
        System sys;
        sys << a + b + c - 6;
        sys << (a ^ 2) + (b ^ 2) + (c ^ 2) - 14;
        sys << (a ^ 3) + (b ^ 3) + (c ^ 3) - 36;
        sys << a * b * c - 6;
        auto _ = sys.Solve(a);
        BOOST_TEST(_.size() == 1);
    }
    {
        DECL_VARS(a, b, c);
        System sys;
        sys << a + b + c - 6;
        sys << (a ^ 2) + (b ^ 2) + (c ^ 2) - 14;
        sys << (a ^ 3) + (b ^ 3) + (c ^ 3) - 36;
        sys << a * b * c - 6;
        sys << a + b - 4;
        auto _ = sys.Solve(a);
        BOOST_TEST(_.size() == 1);
    }
    {
        DECL_VARS(a, b, c);
        System sys;
        sys << a + b + c - 6;
        sys << (a ^ 2) + (b ^ 2) + (c ^ 2) - 14;
        sys << (a ^ 3) + (b ^ 3) + (c ^ 3) - 36;
        sys << a * b * c - 6;
        sys << a + b - 4;
        sys << a - 2;
        auto _ = sys.Solve(a);
        BOOST_TEST(_.size() == 1);
    }
    {
        DECL_VARS(x, y, z);
        System sys;
        sys << "(x^2)+(y^2)=8"_v
            << "x*y=7"_v
            << "(x+y)^2=z"_v;
        auto _ = sys.Solve(z);
    }
}


BOOST_AUTO_TEST_CASE(System_tests
    , *disabled() // FIXME:
) {
    DECL_VA(a);
    DECL_VA(b);
    DECL_VA(c);
    {
        System sys;
        Valuable t;
        t.SetView(Valuable::View::Equation);
        t = a - 8 - b;
        BOOST_TEST(t.GetView() == Valuable::View::Equation);
        sys << t;
        sys << a + b - 100;
        auto s = sys.Solve(a);
        auto hasOneSolution = s.size()==1;
        BOOST_TEST(hasOneSolution);
        if(hasOneSolution)
        {
            auto _ = *s.begin();
            BOOST_TEST(_ == 54);
        }
    }
    
    {
        System s;
        s << a - 8 - b;
        s << a + b - 21;
        auto _ = s.Solve(a);
        BOOST_TEST(_.size()==1);
    }
    
    {   // more grady system
        DECL_VA(x);
        System s;
        s << (a + b + c).Equals(6);
        s << ((a^2) + (b^2) + (c^2)).Equals(14);
        s << ((a^3) + (b^3) + (c^3)).Equals(36);
        s << x.Equals(a*b*c);
        auto _ = s.Solve(x);
        BOOST_TEST(_.size()==1);
    }
}

BOOST_AUTO_TEST_CASE(Balancing_system_no_hang_test
    , *timeout(20)
) {
    DECL_VARS(x,y,z);
    System() << (constants::minus_1 / 4)*((-16*(y^2)+160*y-8*x-200) ^ constants::half) + z -35;
}

BOOST_AUTO_TEST_CASE(Sudoku_test_task
    , *disabled() // FIXME: 
) // solve sudoku through a system of equations solver
{
    Variable X, Y, v;

    constexpr unsigned Sz = 9;
    int data[Sz][Sz] = {
		{0, 1, 3, 8, 0, 0, 4, 0, 5}, 
		{0, 2, 4, 6, 0, 5, 0, 0, 0},
		{0, 8, 7, 0, 0, 0, 9, 3, 0},
		{4, 9, 0, 3, 0, 6, 0, 0, 0}, 
		{0, 0, 0, 1, 0, 5, 0, 0, 0}, 
		{0, 0, 0, 7, 0, 1, 0, 9, 3},
		{0, 6, 9, 0, 0, 0, 7, 4, 0}, 
		{0, 0, 0, 2, 0, 7, 6, 8, 0}, 
		{1, 0, 2, 0, 0, 8, 3, 5, 0}
	};
    Variable variables[Sz][Sz];
    Valuable expressions[Sz][Sz];

    // sudoku
    System s;
    auto possibleValues = v.Abet({1, 2, 3, 4, 5, 6, 7, 8, 9});
    s << possibleValues;

    // known values
    for (auto rowIdx = Sz; rowIdx--;) {
        for (auto colIdx = Sz; colIdx--;) {
            auto& i = data[rowIdx][colIdx];
            if (i) {
                expressions[colIdx][rowIdx] = i;
            } else {
                expressions[colIdx][rowIdx] = possibleValues;
            }
            s << variables[colIdx][rowIdx].Equals(expressions[colIdx][rowIdx]);
        }
    }

    // rules
    auto inRow = [&](auto& a, auto i, auto x, auto y) -> decltype(a[0][0])& { return a[x * 3 + y][i]; };
    auto inCol = [&](auto& a, auto i, auto x, auto y) -> decltype(a[0][0])& { return a[i][x * 3 + y]; };
    auto inSq = [&](auto& a, auto i, auto x, auto y) -> decltype(a[0][0])& {
        auto sqNx = i % 3;
        auto sqNy = (i - sqNx) / 3;
        return a[sqNx * 3 + x][sqNy * 3 + y];
    };
    auto inequals = [&](auto& accessItem, auto i, auto x, auto y) {
        auto& expression = accessItem(expressions, i, x, y);
        auto found = expression.IsInt();
        auto& variable = accessItem(variables, i, x, y);
        for (int n = Sz; i--;) {
            if (n == i)
                continue;
            auto& otherItemExpression = accessItem(expressions, n, x, y);
            if (found) {
                auto otherItemFound = otherItemExpression.IsInt();
                if (otherItemFound)
                    continue;
            }
            auto& otherItemXY = accessItem(variables, n, x, y);
            auto diff = variable - otherItemXY;
            auto inequality = diff.Equals(1) || diff.Equals(2) || diff.Equals(3) || diff.Equals(4) || diff.Equals(5) ||
                              diff.Equals(6) || diff.Equals(7) || diff.Equals(8) || diff.Equals(-1) ||
                              diff.Equals(-2) || diff.Equals(-3) || diff.Equals(-4) || diff.Equals(-5) ||
                              diff.Equals(-6) || diff.Equals(-7) || diff.Equals(-8);
            s << inequality;
        }
    };
    for (int x = 3; x--;) {
        for (int y = 3; y--;) {
            for (int i = Sz; i--;) {
                // inequalities
                inequals(inRow, i, x, y);
                inequals(inCol, i, x, y);
                inequals(inSq, i, x, y);
            }
        }
    }

    // solve
    for (auto rowIdx = Sz; rowIdx--;) {
        for (auto colIdx = Sz; colIdx--;) {
            auto& exp = expressions[rowIdx][colIdx];
            if (!exp.IsInt()) {
                auto& var = variables[rowIdx][colIdx];
                auto solutions = s.Solve(var);
                BOOST_TEST(solutions.size() == 1);
                auto solution = *solutions.begin();
                exp = solution;
            }
            std::cout << setw(2) << exp;
        }
        std::cout << std::endl;
    }

}

BOOST_AUTO_TEST_CASE(sq_System_test
                     , *disabled()
                     )
{
    System s;
    DECL_VARS(a, b, x);
    s << a - b - 3
        << a + b - x
        << a * b * 4 - (49 - 9)
        << x * x - 49
        << x*b + 9 +2*a*b + 3*b - 49
        ;

    auto _ = s.Solve(x);
    BOOST_TEST(_.size()==1);
    if (_.size() >= 1) {
        auto sqx = *_.begin();
        BOOST_TEST(sqx == 7);
    }
}

BOOST_AUTO_TEST_CASE(Quadratic_System_test
    , *disabled() // Enable after review
) {
    DECL_VARS(l);
    System sys;
    
    // 9l^2 - 2 = 16
    // Rearranged to: 9l^2 - 18 = 0
    sys << 9*(l^2) - 18;
    
    auto solutions = sys.Solve(l);
    BOOST_TEST(solutions.size() == 2);  // Should have both +√2 and -√2
    
    if (solutions.size() == 2) {
        // Get both solutions
        auto it = solutions.begin();
        auto sol1 = *it++;
        auto sol2 = *it;
        
        // Verify solutions satisfy l^2 = 2
        BOOST_TEST((sol1^2) == 2);
        BOOST_TEST((sol2^2) == 2);
        
        // Verify one is positive and one is negative
        BOOST_TEST((sol1 * sol2) == -2);
        
        // Calculate l^3 for both solutions
        auto l1_cubed = sol1^3;
        auto l2_cubed = sol2^3;
        
        // l^3 should be ±2√2
        BOOST_TEST(l1_cubed == 2*sol1);
        BOOST_TEST(l2_cubed == 2*sol2);
    }
}

BOOST_AUTO_TEST_CASE(ComplexSystem_test, *disabled()) // TODO :
{
    // https://github.com/ohhmm/openmind/issues/8
    // In a farm there are 100 animals consisting of cows,goats,and buffalos.
    // each goat gives 250g of milk,each Buffalo gives 6kg of milk and if each cow gives 4kg of milk.
    // if in total 100 animals produce 40 kg of milk how many animals of each type are present?
    // https://www.quora.com/Can-this-math-problem-be-solved
    System s;
    Variable c,g,b;
    s   << c+g+b-100
        << g*250+b*6000+c*4000-40000;
    
    // c,g,b are integers, see https://math.stackexchange.com/a/1598552/118612
    // TODO : try (x mod 1) or cos(2*PI*x) instead
//    s << c.MustBeInt() << g.MustBeInt() << b.MustBeInt();
    s << c.GreaterOrEqual(0);
    s << g.GreaterOrEqual(0);
    s << b.GreaterOrEqual(0);

    auto _ = s.Solve(c);
    BOOST_TEST(_.size() > 0);
    _ = s.Solve(g);
    BOOST_TEST(_.size() > 0);
    _ = s.Solve(b);
    BOOST_TEST(_.size() > 0);
    //    auto cc = s.SolveSingleInteger(c);
//    auto gc = s.SolveSingleInteger(g);
//    auto bc = s.SolveSingleInteger(b);
//    BOOST_TEST(values);
}

BOOST_AUTO_TEST_CASE(hello_sudoku_world
                     ,*disabled()
                    )
{
    constexpr unsigned Sz = 9;
    int data[Sz][Sz] = {
        {0, 9, 8, 0, 4, 0, 0, 0, 0},
        {4, 2, 0, 0, 9, 0, 0, 8, 0},
        {0, 0, 0, 3, 0, 1, 0, 0, 0},
        {6, 3, 9, 0, 0, 8, 7, 0, 0},
        {2, 0, 4, 9, 0, 7, 3, 0, 8},
        {0, 0, 7, 5, 0, 0, 9, 2, 6},
        {0, 0, 0, 4, 0, 3, 0, 0, 0},
        {0, 6, 0, 0, 1, 0, 0, 4, 9},
        {0, 0, 0, 0, 5, 0, 8, 3, 0}
    };
    Variable value[Sz][Sz];
    
    auto f = 1*2*3*4*5*6*7*8*9;
    System world;
    for (auto x=Sz; x--;) {
        Valuable sumx, sumy, mulx(1), muly(1);
        for (auto y=Sz; y--; ) {
            sumx += value[x][y];
            sumy += value[y][x];
            mulx *= value[x][y];
            muly *= value[y][x];
            if (data[x][y]) {
                world << value[x][y].Equals(data[x][y]);
            }
        }
        world << sumx.Equals(45) << sumy.Equals(45)
            << mulx.Equals(f) << muly.Equals(f);
    }

    for (auto x=Sz; x--;) {
        for (auto y=Sz; y--; ) {
            auto& i = value[x][y];
            auto s = world.Solve(i);
            if (s.size()!=1) {
                IMPLEMENT
            }else{
                std::cout << *s.begin() << ' ';
            }
        }
        std::cout << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(Sudoku_simplest_test
                     ,*disabled()
                     ) // solve sudoku through a system of equations though
{
    Valuable s;
    Variable x,y,v;
    constexpr unsigned Sz = 9;
    int data[Sz][Sz] = {
        {0, 9, 8, 0, 4, 0, 0, 0, 0},
        {4, 2, 0, 0, 9, 0, 0, 8, 0},
        {0, 0, 0, 3, 0, 1, 0, 0, 0},
        {6, 3, 9, 0, 0, 8, 7, 0, 0},
        {2, 0, 4, 9, 0, 7, 3, 0, 8},
        {0, 0, 7, 5, 0, 0, 9, 2, 6},
        {0, 0, 0, 4, 0, 3, 0, 0, 0},
        {0, 6, 0, 0, 1, 0, 0, 4, 9},
        {0, 0, 0, 0, 5, 0, 8, 3, 0}
    };
    Variable value[Sz][Sz];
    
    auto at = [&](const Valuable& xx, const Valuable& yy, const Valuable& vv){
        return x.Equals(xx).sq() + y.Equals(yy).sq() + v.Equals(vv).sq();
    };
    
    // define known data
    for(auto rowIdx = Sz; rowIdx--;){
        for(auto colIdx = Sz; colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i)
                s += at(colIdx,rowIdx,i).sq();
        }
    }
    
    // define the universe
    // for simplicity check only row/col/square sum equalities to 45
    for(int xx=0; xx<=8; ++xx){
        auto sumInRow = -45_v;
        auto sumInCol = -45_v;
        auto sumInSq = -45_v;
        for(int yy=0; yy<=8; ++yy){
//            if (data[yy][xx]) {
//                <#statements#>
//            }
//            auto a = at(xx,yy,value)(value);
            sumInRow += at(xx,yy,value[xx][yy])(value[xx][yy]);
            sumInCol += at(yy,xx,value[xx][yy])(value[xx][yy]);
            auto sqn = xx;
            auto sqx = sqn%3;
            auto sqy = (sqn - sqx) / 3;
            auto insqn = yy;
            auto insqx = insqn % 3;
            auto insqy = (insqn - insqx) / 3;
            auto sqxx = sqx*3+insqx;
            auto sqyy = sqy*3+insqy;
            sumInSq += at(sqxx,sqyy,value[sqxx][sqyy])(value[sqxx][sqyy]);
        }
        s += sumInRow ^ 2;
        s += sumInCol ^ 2;
        s += sumInSq ^ 2;
    }
    
//    Valuable::optimizations = true;
//    std::cout << omnn::measure::nanoseconds([&](){
//        s.optimize();
//    }) << std::endl;
//    Valuable::optimizations = {};

    //    std::atomic<int> MaxTasks = boost::thread::hardware_concurrency();
    std::deque<std::future<void>> tasks;
    //    auto ChooseNextTask = [&tasks](){
    //
    //    };
    //    auto AddPoolTask = [](std::function<
    // solving
    auto sysMutex = std::make_shared<boost::shared_mutex>();
    for(auto rowIdx = Sz; rowIdx--;){
        for(auto colIdx = Sz; colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i == 0){
                auto co = s;
                Valuable::OptimizeOn(), co.eval({{x, colIdx},{y, rowIdx}});
                std::cout << co.str() << std::endl;
                auto is = co.IntSolutions(v);
                if(is.size()==1 && is.begin()->IsInt()){
                    i = static_cast<int>(*is.begin());
                    s += at(colIdx, rowIdx, i);
                } else {
                    IMPLEMENT
                }
//                tasks.push_back(
//                                std::async([colIdx, rowIdx, sysMutex,
//                                            &s, &at, &data](){
//                    Variable find;
//                    sysMutex->lock_shared();
//                    decltype(s) localSystem = s;
//                    sysMutex->unlock_shared();
//                    localSystem << at(colIdx,rowIdx, find);
//                    auto solutions = localSystem.Solve(find);
//                    if (solutions.size()) {
//                        if (solutions.size()==1) {
//                            auto solution = *solutions.begin();
//                            if (!solution.IsInt()) {
//                                IMPLEMENT
//                            }
//                            else
//                            {
//                                data[rowIdx][colIdx] = static_cast<int>(solution);
//                                auto item = at(colIdx,rowIdx,solution);
//                                sysMutex->lock();
//                                s << item;
//                                sysMutex->unlock();
//                            }
//                        }
//                        else
//                        {
//                            // intersect into possible solutions array for this x,y
//                            auto values = 1_v;
//                            for(auto& solution: solutions){
//                                values.logic_or(at(colIdx,rowIdx,solution));
//                            }
//                            sysMutex->lock();
//                            s<<values;
//                            sysMutex->unlock();
//                        }
//                    }
//                }));
            }
        }
    }
    do {
        tasks.pop_front();
    } while (tasks.size());
    
    ofstream o(TEST_SRC_DIR"sudoku.txt", fstream::out);
    for(auto rowIdx = Sz; rowIdx--;){
        for(auto colIdx = Sz; colIdx--;){
            auto& i = data[colIdx][rowIdx];
            BOOST_TEST(i != 0);
            o << setw(2) << i;
        }
        o << endl;
    }
    o.close();
}

BOOST_AUTO_TEST_CASE(Sudoku_system_test
                     ,*disabled()
) // solve sudoku through a system of equations though
{
    System s;
    Variable x,y,v;
    constexpr unsigned Sz = 9;
    int data[Sz][Sz] = {
        {0, 9, 8, 0, 4, 0, 0, 0, 0},
        {4, 2, 0, 0, 9, 0, 0, 8, 0},
        {0, 0, 0, 3, 0, 1, 0, 0, 0},
        {6, 3, 9, 0, 0, 8, 7, 0, 0},
        {2, 0, 4, 9, 0, 7, 3, 0, 8},
        {0, 0, 7, 5, 0, 0, 9, 2, 6},
        {0, 0, 0, 4, 0, 3, 0, 0, 0},
        {0, 6, 0, 0, 1, 0, 0, 4, 9},
        {0, 0, 0, 0, 5, 0, 8, 3, 0}
    };
    
    // at
    auto at = [&](const Valuable& xx, const Valuable& yy, const Valuable& vv) {
        return x.Equals(xx).sq() + y.Equals(yy).sq() + v.Equals(vv).sq();
    };
    
    // define known data
    for(auto rowIdx = Sz; rowIdx--;){
        for(auto colIdx = Sz; colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i)
                s << at(colIdx,rowIdx,i);
        }
    }
    
    // define the universe
    // for simplicity check only row/col/square sum equalities to 45
    Variable value;
    for(int xx=0; xx<=8; ++xx){
        auto sumInRow = -45_v;
        auto sumInCol = -45_v;
        auto sumInSq = -45_v;
        for(int yy=0; yy<=8; ++yy){
            sumInRow += at(xx,yy,value)(value);
            sumInCol += at(yy,xx,value)(value);
            auto sqn = xx;
            auto sqx = sqn%3;
            auto sqy = (sqn - sqx) / 3;
            auto insqn = yy;
            auto insqx = insqn % 3;
            auto insqy = (insqn - insqx) / 3;
            sumInSq += at(sqx * 3 + insqx, sqy * 3 + insqy, value)(value);
        }
        s << sumInRow << sumInCol << sumInSq;
    }
    
    std::deque<std::future<void>> tasks;

	// solving
    auto sysMutex = std::make_shared<boost::shared_mutex>();
    for(auto rowIdx = Sz; rowIdx--;){
        for(auto colIdx = Sz; colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i==0){
                tasks.push_back(
                                std::async([colIdx, rowIdx, sysMutex,
                                            &s, &at, &data](){
                    Variable find;
                    sysMutex->lock_shared();
                    decltype(s) localSystem = s;
                    sysMutex->unlock_shared();
                    localSystem << at(colIdx,rowIdx, find);
                    auto solutions = localSystem.Solve(find);
                    if (solutions.size()) {
                        if (solutions.size()==1) {
                            auto solution = *solutions.begin();
                            if (!solution.IsInt()) {
                                IMPLEMENT
                            }
                            else
                            {
                                data[rowIdx][colIdx] = static_cast<int>(solution);
                                auto item = at(colIdx,rowIdx,solution);
                                sysMutex->lock();
                                s << item;
                                sysMutex->unlock();
                            }
                        }
                        else
                        {
                            // intersect into possible solutions array for this x,y
                            auto values = 1_v;
                            for(auto& solution: solutions){
                                values.logic_or(at(colIdx,rowIdx,solution));
                            }
                            sysMutex->lock();
                            s<<values;
                            sysMutex->unlock();
                        }
                    }
                }));
            }
        }
    }
    do {
        tasks.pop_front();
    } while (tasks.size());

	for (auto& row : data) {
        for (auto& cell : row) {
            BOOST_TEST(cell != 0);
            std::cout << cell << ' ';
        }
        std::cout << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(Sudoku_test
                     ,*disabled()
                     ) // solve sudoku through a system of equations though
{
    System s;
    Variable x,y,v;
    std::array<std::array<int, 9>, 9> data {{
        {0, 9, 8, 0, 4, 0, 0, 0, 0},
        {4, 2, 0, 0, 9, 0, 0, 8, 0},
        {0, 0, 0, 3, 0, 1, 0, 0, 0},
        {6, 3, 9, 0, 0, 8, 7, 0, 0},
        {2, 0, 4, 9, 0, 7, 3, 0, 8},
        {0, 0, 7, 5, 0, 0, 9, 2, 6},
        {0, 0, 0, 4, 0, 3, 0, 0, 0},
        {0, 6, 0, 0, 1, 0, 0, 4, 9},
        {0, 0, 0, 0, 5, 0, 8, 3, 0}
    }};

    // at
    auto at = [&](auto& xx, auto& yy, auto& vv){
        return x.Equals(xx).LogicAnd(y.Equals(yy)).LogicAnd(v.Equals(vv));
    };

    // define known data
    for(auto rowIdx = data.size(); rowIdx--;){
        for(auto colIdx = data[rowIdx].size(); colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i)
                s << at(colIdx,rowIdx,i);
        }
    }

    // define possible values
    auto abet = [](const Variable& x) {
        auto v = 1_v;
        for(int i=1; i<10; ++i){
            v.logic_or(x.Equals(i));
        }
        return v;
    };

    // define the universe
     // each item
    for(int xx=0; xx<=8; ++xx){
        for(int yy=0; yy<=8; ++yy){
            for(int val=0; val<=8; ++val)
            {
                auto valsExceptVal = abet(v)/v.Equals(val);
                auto valAt = at(x,y,val);

                // inequality in row
                for(auto leastCorowers = xx + 1; leastCorowers <= 8; ++leastCorowers){
                    auto siblingSameVal = at(leastCorowers,y,val);
                    auto siblingExceptVal = 1_v;
                    for(int exceptVal = 0; exceptVal<=8; ++exceptVal){
                        if(exceptVal!=val)
                            siblingExceptVal.logic_or(at(leastCorowers,y,exceptVal));
                    }
                    s << valAt.Ifz(siblingExceptVal,siblingExceptVal.LogicOr(siblingSameVal));
                }

                // inequality in column
                for(auto leastCocolumners = yy + 1; leastCocolumners <= 8; ++leastCocolumners){
                    auto siblingSameVal = at(leastCocolumners,y,val);
                    auto siblingExceptVal = 1_v;
                    for(int exceptVal = 0; exceptVal<=8; ++exceptVal){
                        if(exceptVal!=val)
                            siblingExceptVal.logic_or(at(leastCocolumners,y,exceptVal));
                    }
                    s << valAt.Ifz(siblingExceptVal,siblingExceptVal.LogicOr(siblingSameVal));
                }

                // inequality in square
                auto sqxStart = xx-(xx%3);
                for(int sqx = sqxStart; sqx < sqxStart+3; ++sqx){
                    auto sqyStart = yy-(yy%3);
                    for(int sqy = sqyStart; sqy < sqyStart+3; ++sqy){
                        if(!(sqx==xx && sqy==yy)){
                            auto siblingSameVal = at(sqx,sqy,val);
                            auto siblingExceptVal = 1_v;
                            for(int exceptVal = 0; exceptVal<=8; ++exceptVal){
                                if(exceptVal!=val)
                                    siblingExceptVal.logic_or(at(sqx,sqy,exceptVal));
                            }
                            s << valAt.Ifz(siblingExceptVal,siblingExceptVal.LogicOr(siblingSameVal));
                        }
                    }
                }
            }
        }
    }

    // x,y,v are integers, see https://math.stackexchange.com/a/1598552/118612
    //    using namespace constant;
    //    s   << (e^(2*pi*i*x))-1
    //        << (e^(2*pi*i*y))-1
    //        << (e^(2*pi*i*v))-1
    //    ;
    
    // inequality in squares
// todo: try general using coordinatesMultiplicityTo3
//    Variable gaugeX, gaugeY;
//    auto coordinatesMultiplicityTo3 = logicAnd(gaugeX % 3, gaugeY % 3);
//    auto notEqual = [](auto& e1, auto&e2){
//IMPLEMENT
//    };
//    s << logicAnd(coordinatesMultiplicityTo3,
//            logicOr({
//                logicAnd(equality(gaugeX,x), equality(gaugeY,y))
//            })
//            )
     // inequality in squares
//first attept
//    for(int sqareStartX = 0; sqareStartX < 9; sqareStartX += 3){
//        for(int sqareStartY = 0; sqareStartY < 9; sqareStartY += 3){
//            for(int i=1; i<=9; ++i) {
//                auto valsExcept = [&](auto& va, auto& exceptValue){
//                    Valuable allItems=1_v;
//                    for(int xi = sqareStartX; xi < sqareStartX+3; ++xi){
//                        for(int yj = sqareStartY; yj < sqareStartY+3; ++yj){
//                            for(int ii = 1; ii<=9; ++ii)
//                            {
//                                if(ii!=i){
//                                    allItems = allItems.LogicOr(at(xi,yj,));
//                                } else {
//
//                                }
//                            }
//                            auto item = equals(x,xi)
//                                    .LogicAnd(equals(y,yj))
//                                    .LogicAnd(equals(v,i));
//
//                        }
//                    }
//                    return abet(va) / va.Equals(exceptValue);
//                };
//                auto valsExceptI = valsExcept(v,i);
//                s << (x.Equals(sqareStartX).LogicAnd(y.Equals(sqareStartY)))
//                    .LogicAnd();
//            }
//        }
//    }

    // solving
    std::deque<std::future<void>> tasks;
    for(auto rowIdx = data.size(); rowIdx--;){
        for(auto colIdx = data[rowIdx].size(); colIdx--;){
            auto& i = data[rowIdx][colIdx];
            if(i==0){
                auto sysMutex = std::make_shared<boost::shared_mutex>();
                std::function<void()> addThisTask = [&tasks,colIdx,rowIdx,sysMutex,&s,&at,&addThisTask](){
                    tasks.push_back(
                        std::async([colIdx,rowIdx, sysMutex, &s, &at, addThisTask](){
                        Variable find;
                        sysMutex->lock_shared();
                        decltype(s) localSystem = s;
                        sysMutex->unlock_shared();
                        localSystem << at(colIdx,rowIdx, find);
                        auto solutions = localSystem.Solve(find);
                        if (solutions.size()) {
                            if (solutions.size()==1) {
                                auto solution = *solutions.begin();
                                if (!solution.IsInt()) {
                                    IMPLEMENT
                                }
                                else
                                {
                                    auto item = at(colIdx,rowIdx,solution);
                                    sysMutex->lock();
                                    s << item;
                                    sysMutex->unlock();
                                }
                            }
                            else
                            {
                                // todo : intersect into possible solutions array for this x,y
                                for(auto& solution: solutions){
                                    
                                }
                            }
                        }
                    }));
                };
            }
        }
    }


//    auto _ = s.Solve(c);
//    auto cc = s.SolveSingleInteger(c);
//    auto gc = s.SolveSingleInteger(g);
//    auto bc = s.SolveSingleInteger(b);
//    BOOST_TEST(values);
}

BOOST_AUTO_TEST_CASE(kaggle_test, *disabled())
{
    using namespace boost;
    using namespace boost::executors;
    basic_thread_pool tp;

    //TypedVarHost<std::string> vh;

    std::atomic<int> cntLines = 0;
    std::map<int, std::string> ids;
    std::string line;
    {
    ifstream in(TEST_SRC_DIR"train.csv", ifstream::in);
    if (!in.is_open())
    {
        cout << "cannot open file\n";
        return;
    }

    std::vector<Variable> v;
    System sys;
    std::mutex systemMutex;
    using namespace boost;

    if (!in.eof()) {
        in >> line; // headers
        tokenizer<escaped_list_separator<char> > tk(
            line, escaped_list_separator<char>('\\', ',', '\"'));
        for (tokenizer<escaped_list_separator<char> >::iterator i(tk.begin());
            i != tk.end(); ++i)
            v.push_back(Variable());

#ifdef _WIN32
        EnableMenuItem(
            GetSystemMenu(GetConsoleWindow(), {}),
            SC_CLOSE,
            MF_GRAYED);
#endif // _WIN32
    }

    if (!in.eof())
        ++cntLines;

    int skip = 0;
    {
        ifstream system(TEST_SRC_DIR"sys.csv", fstream::in);
        if (system.is_open())
        {
            while (!system.eof()) {
                constexpr auto Sz = 1 << 18;
                char s[Sz];
                system.getline(s, Sz);
                line = s;
                if (!line.empty())
                {
                    ++skip;
                    auto c = memchr(s, ';', Sz);
                    *(char*)c = 0;
                    ids[skip] = s;
                }
            }
            system.close();
        }
        else
        {
            cout << "cannot open file, creating new";
            ofstream(TEST_SRC_DIR"sys.csv", fstream::out).close();
            return;
        }
    }

    std::atomic<int> completedLines = skip;
    while (!in.eof()) {
        in >> line;
        if (!in.eof())
            ++cntLines;
        if (completedLines + 1 >= cntLines)
            continue;
        tp.submit([&, line]()
        {
            auto sum = std::make_shared<Sum>();
            sum->SetView(Valuable::View::Equation);
            auto sumMutex = std::make_shared<std::mutex>();

            auto cntWords = std::make_shared<std::atomic<int>>(0);
            tokenizer<escaped_list_separator<char> > tk(
                line, escaped_list_separator<char>('\\', ',', '\"'));
            int vi = 0;
            for (tokenizer<escaped_list_separator<char> >::iterator i(tk.begin());
                i != tk.end(); ++i)
            {
                tp.submit([&, sum, sumMutex, cntWords, w = *i, vi](){
                    auto tid = std::this_thread::get_id();
                    std::cout << tid << " start " << vi << std::endl;
                    const Variable& va = v[vi];
                    {
                        Valuable val;
                        if (vi) {
                            val = boost::lexical_cast<double>(w);
                        }
                        else {
                            long long l;
                            std::stringstream ss(w);
                            ss >> hex >> l;
                            val = l;
                        }
                        val = std::move((va - val).sq());

                        std::lock_guard g(*sumMutex);
                        sum->Add(val);
                    }
                    if (cntWords->fetch_add(1) + 1 == v.size())
                    {
                        sum->optimize();

                        {
                            std::lock_guard g(systemMutex);
                            sys << *sum;
                            ofstream system(TEST_SRC_DIR"sys.csv", fstream::app);
                            system << completedLines << ';' << *sum << endl;
                        }
                        std::cout << tid << " line complete " << completedLines << std::endl;
                        if (completedLines.fetch_add(1) + 1 == cntLines)
                        {
                            // save sys and start processing test lines file
                            std::cout << tid << " sys complete " << std::endl;
                        }
                    }
                    std::cout << tid << " complete " << vi << std::endl;
                });
                ++vi;
            }
        });
    }

    in.close();
    }
    tp.join();

    {
        ifstream in(TEST_SRC_DIR"train.csv", ifstream::in);
        ifstream system(TEST_SRC_DIR"sys.csv", fstream::in);
        ifstream test(TEST_SRC_DIR"test.csv", ifstream::in);

        if (!in.is_open() || !system.is_open())
        {
            cout << "cannot open file\n";
            return;
        }
        auto h = VarHost::make<std::string>();
        while (!system.eof()) {
            constexpr auto Sz = 1 << 18;
            char s[Sz];
            system.getline(s, Sz);
            line = std::string(s);
            if (!line.empty())
            {
                std::size_t sc = line.find(';');
                line = line.substr(sc + 1, line.length() - sc);
                Valuable v(line, h);
                BOOST_TEST(v.str() == line);
            }
        }
    }

    tp.join();
}

BOOST_AUTO_TEST_CASE(System_empty_test) {}
