import unittest
import valuable

class TestValuable(unittest.TestCase):
    def setUp(self):
        """Set up test variables."""
        self.x = valuable.Variable("x")
        self.y = valuable.Variable("y")
        self.x.set_value(3)
        self.y.set_value(4)
        
        self.a = valuable.Valuable(5)
        self.b = valuable.Valuable(3)
    
    def test_variable_creation_and_value_setting(self):
        """Test variable creation and value assignment."""
        self.assertEqual(float(self.x), 3.0)
        self.assertEqual(float(self.y), 4.0)
    
    def test_variable_arithmetic(self):
        """Test arithmetic operations between variables."""
        # Addition
        self.assertEqual(float(self.x + self.y), 7.0)
        self.assertEqual(float(self.y + self.x), 7.0)
        
        # Subtraction
        self.assertEqual(float(self.y - self.x), 1.0)
        self.assertEqual(float(self.x - self.y), -1.0)
        
        # Multiplication
        self.assertEqual(float(self.x * self.y), 12.0)
        self.assertEqual(float(self.y * self.x), 12.0)
        
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
