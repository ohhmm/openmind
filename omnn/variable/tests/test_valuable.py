#!/usr/bin/env python3
import unittest
import sys

try:
    import variable
    VARIABLE_MODULE_AVAILABLE = True
except ImportError:
    VARIABLE_MODULE_AVAILABLE = False

@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
class Testvariable(unittest.TestCase):
    def test_variable_creation(self):
        x = variable.Variable()
        v = variable.Valuable("x")
    
    def test_basic_arithmetic(self):
        x = variable.Variable()
        y = variable.Variable()
        sum = x + y
        values = {y: variable.Valuable(2), x: variable.Valuable(3)}
        # Test multiplication
        prod = y * 2 + x + 1
        prod.eval(values)
        prod.optimize()
        #prod = variable.Valuable("4 * 2")
        self.assertEqual(float(prod), 8.0)  # 4 * 2
        
        # Test addition with expression
        expr = x + prod
        expr.eval(values)
        expr.optimize()
        self.assertEqual(float(expr), 11.0)  # 3 + (4 * 2)
        
        # Test reverse operations
        rev_prod = 2 * y.sq()
        rev_prod.eval(values)
        rev_prod.optimize()
        self.assertEqual(float(rev_prod), 8.0)
        
        rev_sum = 5 + x
        rev_sum.eval(values)
        rev_sum.optimize()
        self.assertEqual(float(rev_sum), 8.0)  # 5 + 3
    
    def test_arithmetic_operations(self):
        a = variable.Valuable(5)
        b = variable.Valuable(3)
        
        self.assertEqual(float(a + b), 8.0)
        self.assertEqual(float(a - b), 2.0)
        self.assertEqual(float(a * b), 15.0)
        self.assertAlmostEqual(float(a / b), 5.0/3.0)

if __name__ == "__main__":
    unittest.main()
