//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE System test
#include <boost/test/unit_test.hpp>

#include "System.h"
#include "Varhost.h"

#include <boost/thread/thread_pool.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <future>
#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif
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

BOOST_AUTO_TEST_CASE(kaggle_test, *disabled())
{
    boost::basic_thread_pool tp;

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
                    auto c = memchr(s, ',', Sz);
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
