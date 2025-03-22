#define BOOST_TEST_MODULE ODE test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "System.h"
#include "Valuable.h"
#include "generic.hpp"

using namespace omnn::math;
using namespace boost::unit_test;

// Test case for first-order linear ODE: dy/dx + P(x)y = Q(x)
BOOST_AUTO_TEST_CASE(FirstOrderLinearODE_test)
{
    Variable x = Variable("x");
    Variable y = Variable("y");
    
    // Define the ODE: dy/dx + 2y = x
    // The general solution is y = (x/2 - 1/4) + Ce^(-2x)
    
    // Create the differential equation
    auto dy_dx = y.d(x);
    auto ode = dy_dx + 2*y - x;
    
    // Verify the equation is properly formed
    std::cout << "ODE: " << ode << std::endl;
    
    // Test a particular solution
    auto particular_solution = (x/2) - (1_v/4);
    auto dy_dx_solution = particular_solution.d(x);
    auto verification = dy_dx_solution + 2*particular_solution - x;
    
    verification.optimize();
    BOOST_TEST(verification.IsZero());
}

// Test case for homogeneous first-order ODE: dy/dx = f(y/x)
BOOST_AUTO_TEST_CASE(HomogeneousFirstOrderODE_test
    , *disabled()  // Initially disabled following TDD workflow
)
{
    Variable x = Variable("x");
    Variable y = Variable("y");
    
    // Define the ODE: dy/dx = y/x
    // The general solution is y = Cx
    
    // Create the differential equation
    auto dy_dx = y.d(x);
    auto ode = dy_dx - y/x;
    
    // Verify the equation is properly formed
    std::cout << "Homogeneous ODE: " << ode << std::endl;
    
    // Test a particular solution (y = 2x)
    auto solution = 2*x;
    auto dy_dx_solution = solution.d(x);
    auto verification = dy_dx_solution - solution/x;
    
    verification.optimize();
    BOOST_TEST(verification.IsZero());
}

// Test case for second-order ODE: d²y/dx² + a(dy/dx) + by = 0
BOOST_AUTO_TEST_CASE(SecondOrderODE_test
    , *disabled()  // Initially disabled following TDD workflow
)
{
    Variable x = Variable("x");
    Variable y = Variable("y");
    
    // Define the ODE: d²y/dx² + 4(dy/dx) + 4y = 0
    // The general solution is y = (C1 + C2x)e^(-2x)
    
    // Create the differential equation
    auto dy_dx = y.d(x);
    auto d2y_dx2 = dy_dx.d(x);
    auto ode = d2y_dx2 + 4*dy_dx + 4*y;
    
    // Verify the equation is properly formed
    std::cout << "Second-order ODE: " << ode << std::endl;
    
    // Test a particular solution (y = xe^(-2x))
    auto solution = x * (constants::e ^ (-2*x));
    auto dy_dx_solution = solution.d(x);
    auto d2y_dx2_solution = dy_dx_solution.d(x);
    auto verification = d2y_dx2_solution + 4*dy_dx_solution + 4*solution;
    
    verification.optimize();
    BOOST_TEST(verification.IsZero());
}

// Test case for solving ODEs using the System class
BOOST_AUTO_TEST_CASE(SolveODE_test
    , *disabled()  // Initially disabled following TDD workflow
)
{
    Variable x = Variable("x");
    Variable y = Variable("y");
    Variable C = Variable("C");
    
    // Define the ODE: dy/dx = y
    // The general solution is y = Ce^x
    
    // Create the differential equation
    auto dy_dx = y.d(x);
    auto ode = dy_dx - y;
    
    // Set up the system
    System sys;
    sys << ode;
    
    // Add an initial condition: y(0) = 1
    Valuable y_at_0 = y(x, 0);
    sys << y_at_0.Equals(1);
    
    // Solve for the constant of integration
    auto solutions = sys.Solve(C);
    
    // Verify we have a solution
    BOOST_TEST(solutions.size() == 1);
    
    // Verify the solution satisfies the ODE
    auto solution = constants::e ^ x;
    auto verification = solution.d(x) - solution;
    verification.optimize();
    BOOST_TEST(verification.IsZero());
}

// Test case for numerical approximation of ODE solutions
BOOST_AUTO_TEST_CASE(NumericalODE_test
    // Enabled to avoid linking issues as per maintainer's request
)
{
    Variable x = Variable("x");
    Variable y = Variable("y");
    
    // Define the ODE: dy/dx = x + y
    // Create the differential equation
    auto dy_dx = y.d(x);
    auto ode = dy_dx - x - y;
    
    // Verify the equation is properly formed
    std::cout << "Numerical ODE: " << ode << std::endl;
    
    // Test a particular solution using numerical evaluation
    // For the ODE dy/dx = x + y with initial condition y(0) = 1,
    // the solution is y = -1 - x + 2e^x
    auto solution = -1_v - x + 2*(constants::e ^ x);
    
    // Verify at several points
    for (int i = 0; i <= 5; i++) {
        auto x_val = i;
        auto y_val = solution.Eval({{x, x_val}});
        auto dy_dx_val = (x_val + y_val);  // From the ODE: dy/dx = x + y
        
        // Calculate the derivative numerically for verification
        auto y_next = solution.Eval({{x, x_val + 0.001}});
        auto y_prev = solution.Eval({{x, x_val - 0.001}});
        auto numerical_derivative = (y_next - y_prev) / 0.002;
        
        // Verify the derivative matches the ODE
        auto error = (numerical_derivative - dy_dx_val).abs();
        BOOST_TEST(error < Valuable(0.01));  // Allow small numerical error
    }
}
