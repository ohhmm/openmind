#define BOOST_TEST_MODULE 8Queens test
#include <boost/test/unit_test.hpp>

#include "System.h"
#include "VarHost.h"
#include "e.h"
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


namespace omnn::chess {

class X : public Variable{
    using base = Variable;
public:
    using base::base;
    static constexpr char Columns[] = {'a','b','c','d','e','f','g','h'};
    static constexpr auto ColNo(char c) { return c-Columns[0]; }
    void Rules(System& s) const {
        s << Valuable::Abet(*this, {'a','b','c','d','e','f','g','h'});
    }
};

class Y : public Variable{
    using base = Variable;
public:
    using base::base;

    void Rules(System& s) const {
        s << Valuable::Abet(*this, {1,2,3,4,5,6,7,8});
    }
};

class VaField : std::array<std::array<Variable, 8>, 8>{
public:
    friend System& operator<<(System& s, const VaField& vaField){
        for (auto column : X::Columns) {
            auto& col = vaField[X::ColNo(column)];
            for (int varIdx = vaField.size(); varIdx-->0; ) {
                auto& va = vaField[varIdx];

            }
        }
    }
};

class Field : std::array<std::array<Valuable, 8>, 8>{
    Valuable step = 0;
    using base = std::array<std::array<Valuable, 8>, 8>;

public:
    enum{White=0,Black=1};
    enum{
        King=1<<1,
        Queen=2<<1,
        Pawn=3<<1,
        Knight=4<<1,
        Bishop=5<<1,
        Rook=6<<1,
    };
//    Field():base{
//        {Rook & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Rook & Black},
//        {Knight & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Knight & Black},
//        {Bishop & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Bishop & Black},
//        {Queen & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Queen & Black},
//        {King & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, King & Black},
//        {Bishop & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Bishop & Black},
//        {Knight & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Knight & Black},
//        {Rook & White, Pawn & White, 0, 0, 0, 0, Pawn & Black, Rook & Black},
//    }{
//    }

    virtual void Rules(System& s) const {
        throw;
    }
};

class Figure : public Variable{
    using base = Variable;
    const X& x_;
    const Y& y_;
protected:
    using base::base;
public:
    Figure(const X& x, const Y& y)
        : x_(x), y_(y)
    { }

    virtual void Rules(System& s) const {
        x_.Rules(s);
        y_.Rules(s);
    }
};

class P : public Figure{
    using base = Figure;
public:
    using base::base;

    void Rules(System& s) const override{
        base::Rules(s);

    }

    Valuable Moves(){
        throw;
    }
};

class B : public Figure{
    using base = Figure;
public:
    using base::base;

    void Rules(System& s) const override{
        base::Rules(s);

    }
};


}

BOOST_AUTO_TEST_CASE(Solve8Qpuzzle)
{
    omnn::chess::Field field;
//    omnn::chess::Q queens[8];

    System sys;

}
