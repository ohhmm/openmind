//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE System test
#include <boost/test/unit_test.hpp>

#include "System.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <future>
#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <thread>
using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(System_tests)
{
    {
        System sys;
        Variable a,b;
        Valuable t;
        t.SetView(Valuable::View::Equation);
        t = a - 8 - b;
        sys << t;
        sys << a + b - 100;
        auto s = sys.Solve(a);
        BOOST_TEST(s.size()==1);
        auto _ = *s.begin();
        BOOST_TEST(_ == 54);
    }
    
    {
        System s;
        Variable a,b;
        s << a - 8 - b;
        s << a + b - 21;
        auto _ = s.Solve(a);
        BOOST_TEST(_.size()==1);
    }
}

BOOST_AUTO_TEST_CASE(sq_System_test, *disabled())
{
    System s;
    Variable a,b,x;
	s << a - b - 3
		<< a + b - x
		<< a * b * 4 - (49 - 9)
		//<< x * x - 49
        << x*b + 9 +2*a*b + 3*b - 49
        ;

    auto _ = s.Solve(x);
    BOOST_TEST(_.size()==1);
	auto sqx = *_.begin();
	BOOST_TEST(sqx == 7);
}

BOOST_AUTO_TEST_CASE(ComplexSystem_test, *disabled()) // TODO :
{
    // https://github.com/ohhmm/openmind/issues/8
    // In a farm there are 100 animals consisting of cows,goats,and buffalos.each goat gives 250g of milk,each Buffalo gives 6kg of milk and if each cow gives 4kg of milk.if in total 100 animals produce 40 kg of milk how many animals of each type are present?
    // https://www.quora.com/Can-this-math-problem-be-solved
    System s;
    Variable c,g,b;
    s   << c+g+b-100
        << g*250+b*6000+c*4000-40000
        // c,g,b are integers, see https://math.stackexchange.com/a/1598552/118612
        // << (e^(2*π*i*c))-1
    ;
//    auto cc = s.SolveSingleInteger(c);
//    auto gc = s.SolveSingleInteger(g);
//    auto bc = s.SolveSingleInteger(b);
//    BOOST_TEST(values);
}

BOOST_AUTO_TEST_CASE(kaggle_test/*, *disabled()*/)
{
    //TypedVarHost<std::string> vh;

    ifstream in(TEST_SRC_DIR"train.csv", ifstream::in);
    if (!in.is_open())
    {
        cout << "cannot open file\n";
        return;
    }

    //std::map<std::string, Variable> v;
    std::vector<Variable> v;
    System s;
    std::mutex m;
    using namespace boost;
    std::string line;
    if (!in.eof()) {
        in >> line; // headers
        tokenizer<escaped_list_separator<char> > tk(
            line, escaped_list_separator<char>('\\', ',', '\"'));
        for (tokenizer<escaped_list_separator<char> >::iterator i(tk.begin());
            i != tk.end(); ++i)
            v.push_back(Variable());
    }
    std::deque<std::future<bool> > tasks;
    //while (!in.eof()) {
        in >> line;
        tokenizer<escaped_list_separator<char> > tk(
            line, escaped_list_separator<char>('\\', ',', '\"'));
        int vi = 0;
        std::deque<std::future<Valuable>> d;
        for (tokenizer<escaped_list_separator<char> >::iterator i(tk.begin());
            i != tk.end(); ++i)
        {
            d.push_back(
                std::async(
                    [&v](int i, auto s) {
                        auto tid = std::this_thread::get_id();
                        std::cout << tid << " start " << i << std::endl;
                        Variable& va = v[i];
                        Valuable val;
                        auto t = s;
                        if (i) {
                            val = boost::lexical_cast<double>(s);
                        }
                        else {
                            long long l;
                            std::stringstream ss(s);
                            ss >> hex >> l;
                            val = l;
                        }
                        std::cout << tid << " complete " << i << std::endl;
                        return (va - val) ^ 2;
                    },
                    vi++,
                    *i
                )
            );
        }

        tasks.push_back(std::async(
            [&](auto&& deq) {
                Sum sum;
                sum.SetView(Valuable::View::Equation);
                for (auto&& de : deq)
                    sum.Add(de.get());
                sum.optimize();

                std::lock_guard g(m);
                s << sum;
                return true;
            },
            std::move(d)));
    //}

    while (tasks.size()) {
        tasks.front().get();
        tasks.pop_front();
    }




    in.close();

}