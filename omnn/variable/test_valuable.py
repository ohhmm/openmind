import unittest
import variable

class Testvariable(unittest.TestCase):
    def test_variable_creation(self):
        x = variable.Variable("x")
        x.set_value(3)
        self.assertEqual(float(x), 3.0)
        
        y = variable.Variable("y")
        y.set_value(4)
        self.assertEqual(float(y), 4.0)
    
    def test_basic_arithmetic(self):
        x = variable.Variable("x")
        x.set_value(3)
        y = variable.Variable("y")
        y.set_value(4)
        
        # Test multiplication
        prod = y * 2
        self.assertEqual(float(prod), 8.0)  # 4 * 2
        
        # Test addition with expression
        expr = x + prod
        self.assertEqual(float(expr), 11.0)  # 3 + (4 * 2)
        
        # Test reverse operations
        rev_prod = 2 * y
        self.assertEqual(float(rev_prod), 8.0)
        
        rev_sum = 5 + x
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
