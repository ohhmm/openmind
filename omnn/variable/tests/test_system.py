#!/usr/bin/env python3
import unittest
import sys

try:
    import variable  # Import the variable module
    VARIABLE_MODULE_AVAILABLE = True
except ImportError:
    VARIABLE_MODULE_AVAILABLE = False

@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
class TestSystem(unittest.TestCase):
    def test_system_creation(self):
        # Create a simple system
        system = variable.System()
        self.assertIsNotNone(system)
    
    def test_system_solve(self):
        # Create variables
        x = variable.Variable()
        y = variable.Variable()
        
        # Create a system of equations
        system = variable.System()
        system << (x + y - 10)  # x + y = 10
        system << (x - y - 2)   # x - y = 2
        
        # Solve for x
        x_solutions = system.Solve(x)
        self.assertEqual(len(x_solutions), 1)
        self.assertEqual(float(x_solutions[0]), 6.0)
        
        # Solve for y
        y_solutions = system.Solve(y)
        self.assertEqual(len(y_solutions), 1)
        self.assertEqual(float(y_solutions[0]), 4.0)

    def test_solve_multivar(self):
        x = variable.Variable()
        y = variable.Variable()
        z = variable.Variable()

        sys = variable.System()

        sys << (x - 3*y + 3*z + 4)
        sys << (2*x + 3*y - z - 15)
        sys << (4*x - 3*y - z - 19)

        sol = sys.Solve(x)
        self.assertEqual(len(sol), 1)
        self.assertEqual(float(sol[0]), 5.0)

        sol = sys.Solve(y)
        self.assertEqual(len(sol), 1)
        self.assertEqual(float(sol[0]), 1.0)

        sol = sys.Solve(z)
        self.assertEqual(len(sol), 1)
        self.assertEqual(float(sol[0]), -2.0)

if __name__ == "__main__":
    unittest.main()
