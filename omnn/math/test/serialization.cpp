#define BOOST_TEST_MODULE Serialization test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Shape {
public:
    virtual ~Shape() = default;
    virtual std::string draw() const = 0;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        // Base class serialization code, if needed
    }
};

class Dot : public Shape {
private:
    friend class boost::serialization::access;
    int x, y;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Shape>(*this);
        ar & x & y;
    }

public:
    Dot(int x = 0, int y = 0) : x(x), y(y) {}
    std::string draw() const override {
        return "Dot at (" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

class Circle : public Shape {
private:
    friend class boost::serialization::access;
    int x, y, radius;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Shape>(*this);
        ar & x & y & radius;
    }

public:
    Circle(int x = 0, int y = 0, int radius = 1) : x(x), y(y), radius(radius) {}
    std::string draw() const override {
        return "Circle at (" + std::to_string(x) + ", " + std::to_string(y) + ") with radius " + std::to_string(radius);
    }
};

class Line : public Shape {
private:
    friend class boost::serialization::access;
    int x1, y1, x2, y2;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Shape>(*this);
        ar & x1 & y1 & x2 & y2;
    }

public:
    Line(int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0) : x1(x1), y1(y1), x2(x2), y2(y2) {}
    std::string draw() const override {
        return "Line from (" + std::to_string(x1) + ", " + std::to_string(y1) + ") to (" + std::to_string(x2) + ", " + std::to_string(y2) + ")";
    }
};

// Export each derived class for Boost.Serialization
BOOST_CLASS_EXPORT(Dot)
BOOST_CLASS_EXPORT(Circle)
BOOST_CLASS_EXPORT(Line)


using namespace omnn::math;
using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE(BoostSerialization_test)
{
    // Create shapes
    std::vector<std::shared_ptr<Shape>> shapes = {
        std::make_shared<Dot>(1, 2),
        std::make_shared<Circle>(3, 4, 5),
        std::make_shared<Line>(6, 7, 8, 9)
    };

    {   // Serialize shapes
        std::ofstream ofs("shapes.dat");
        boost::archive::text_oarchive oa(ofs);
        oa << shapes;
    }

    // Deserialize shapes
    std::vector<std::shared_ptr<Shape>> loaded_shapes;
    std::ifstream ifs("shapes.dat");
    boost::archive::text_iarchive ia(ifs);
    ia >> loaded_shapes;

    // Display loaded shapes
    for (const auto& shape : loaded_shapes) {
        std::cout << shape->draw() << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(ValuableExpressionSerialization_Addition_test)
{
    // Create a Valuable expression for addition
    Valuable a(5);
    Valuable b(3);
    Valuable sum = a + b;

    // Serialize the expression
    std::stringstream ss;
    {
        boost::archive::text_oarchive oa(ss);
        oa << sum;
    }

    // Deserialize the expression
    Valuable loaded_sum;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> loaded_sum;
    }

    // Verify the deserialized expression
    BOOST_CHECK_EQUAL(loaded_sum, sum);
    BOOST_CHECK_EQUAL(loaded_sum, 8);
}

BOOST_AUTO_TEST_CASE(ValuableExpressionSerialization_Subtraction_test)
{
    // Create a Valuable expression for subtraction
    Valuable a(10);
    Valuable b(4);
    Valuable diff = a - b;

    // Serialize the expression
    std::stringstream ss;
    {
        boost::archive::text_oarchive oa(ss);
        oa << diff;
    }

    // Deserialize the expression
    Valuable loaded_diff;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> loaded_diff;
    }

    // Verify the deserialized expression
    BOOST_CHECK_EQUAL(loaded_diff, diff);
    BOOST_CHECK_EQUAL(loaded_diff, 6);
}

BOOST_AUTO_TEST_CASE(ValuableExpressionSerialization_Multiplication_test)
{
    // Create a Valuable expression for multiplication
    Valuable a(6);
    Valuable b(7);
    Valuable product = a * b;

    // Serialize the expression
    std::stringstream ss;
    {
        boost::archive::text_oarchive oa(ss);
        oa << product;
    }

    // Deserialize the expression
    Valuable loaded_product;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> loaded_product;
    }

    // Verify the deserialized expression
    BOOST_CHECK_EQUAL(loaded_product, product);
    BOOST_CHECK_EQUAL(loaded_product, 42);
}

BOOST_AUTO_TEST_CASE(ValuableExpressionSerialization_Division_test)
{
    // Create a Valuable expression for division
    Valuable a(20);
    Valuable b(4);
    Valuable quotient = a / b;

    // Serialize the expression
    std::stringstream ss;
    {
        boost::archive::text_oarchive oa(ss);
        oa << quotient;
    }

    // Deserialize the expression
    Valuable loaded_quotient;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> loaded_quotient;
    }

    // Verify the deserialized expression
    BOOST_CHECK_EQUAL(loaded_quotient, quotient);
    BOOST_CHECK_EQUAL(loaded_quotient, 5);
}

BOOST_AUTO_TEST_CASE(ValuableExpressionSerialization_Exponentiation_test)
{
    // Create a Valuable expression for exponentiation
    Valuable base(2);
    Valuable exponent(3);
    Valuable power = base ^ exponent;

    // Serialize the expression
    std::stringstream ss;
    {
        boost::archive::text_oarchive oa(ss);
        oa << power;
    }

    // Deserialize the expression
    Valuable loaded_power;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> loaded_power;
    }

    // Verify the deserialized expression
    BOOST_CHECK_EQUAL(loaded_power, power);
    BOOST_CHECK_EQUAL(loaded_power, 8);
}
