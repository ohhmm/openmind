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
        
        # Division
        self.assertAlmostEqual(float(self.y / self.x), 4.0/3.0)
        self.assertAlmostEqual(float(self.x / self.y), 3.0/4.0)

    def test_variable_numeric_operations(self):
        """Test arithmetic operations between variables and numbers."""
        # Addition
        self.assertEqual(float(self.x + 2), 5.0)
        self.assertEqual(float(2 + self.x), 5.0)

        # Subtraction
        self.assertEqual(float(self.x - 2), 1.0)
        self.assertEqual(float(2 - self.x), -1.0)

        # Multiplication
        self.assertEqual(float(self.y * 2), 8.0)
        self.assertEqual(float(2 * self.y), 8.0)

        # Division
        self.assertEqual(float(self.y / 2), 2.0)
        self.assertEqual(float(12 / self.y), 3.0)
    def test_valuable_arithmetic(self):
        """Test arithmetic operations between Valuable objects."""
        self.assertEqual(float(self.a + self.b), 8.0)
        self.assertEqual(float(self.a - self.b), 2.0)
        self.assertEqual(float(self.a * self.b), 15.0)
        self.assertAlmostEqual(float(self.a / self.b), 5.0/3.0)

    def test_error_handling(self):
        """Test error handling in arithmetic operations."""
        z = valuable.Variable("z")  # Unassigned variable

        with self.assertRaises(RuntimeError):
            _ = float(z + self.x)  # Operation with unassigned variable should fail

        with self.assertRaises(RuntimeError):
            _ = float(self.y / 0)  # Division by zero should fail

if __name__ == "__main__":
    unittest.main()
