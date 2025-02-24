#!/usr/bin/env python3
import unittest
import variable

#@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
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
        
        print("Testing y * 2")
        prod = y * 2
        print(f"y * 2 = {prod}")
        prod_val = float(prod)
        print(f"y * 2 evaluated = {prod_val}")
        assert prod_val == 8.0  # 4 * 2

        print("Testing x + (y * 2)")
        expr = x + prod
        print(f"Expression: {expr}")
        
        print("Evaluating expression")
        result = expr.evaluate()
        print(f"Result: {result}")
        
        # Convert to float for assertion
        result_float = float(result)
        print(f"Result as float: {result_float}")
        assert result_float == 11.0  # 3 + 4 * 2
        print("Test passed!")
    except Exception as e:
        print(f"Error in test_basic_arithmetic: {e}")
        raise

def test_arithmetic_operations():
    # Test basic operations with constants
    a = valuable.Valuable(5)
    b = valuable.Valuable(3)

    assert float(a + b) == 8.0
    assert float(a - b) == 2.0
    assert float(a * b) == 15.0
    assert float(a / b) == 5.0/3.0

if __name__ == "__main__":
    test_basic_arithmetic()
    test_arithmetic_operations()
    print("All tests passed!")
