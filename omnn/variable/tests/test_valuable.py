#!/usr/bin/env python3
import unittest
import variable

#@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
class TestValuable(unittest.TestCase):
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
        
        self.assertEqual(float(x), 3.0)
        self.assertEqual(float(y), 4.0)

        # Test multiplication
        self.assertEqual(float(y * 2), 8.0)
        self.assertEqual(float(2 * y), 8.0)

        # Test addition
        self.assertEqual(float(x + y), 7.0)
        self.assertEqual(float(5 + x), 8.0)
        
        # Test subtraction
        self.assertEqual(float(y - x), 1.0)
        self.assertEqual(float(5 - x), 2.0)
        
        # Test division
        self.assertEqual(float(y / 2), 2.0)
        self.assertEqual(float(12 / y), 3.0)
    def test_arithmetic_operations(self):
        a = valuable.Valuable(5)
        b = valuable.Valuable(3)

        self.assertEqual(float(a + b), 8.0)
        self.assertEqual(float(a - b), 2.0)
        self.assertEqual(float(a * b), 15.0)
        self.assertAlmostEqual(float(a / b), 5.0/3.0)

if __name__ == "__main__":
    unittest.main()
