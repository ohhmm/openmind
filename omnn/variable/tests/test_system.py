#!/usr/bin/env python3
import unittest
import variable  # Import the variable module

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

if __name__ == "__main__":
    unittest.main()
