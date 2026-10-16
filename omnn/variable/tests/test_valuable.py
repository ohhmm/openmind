#!/usr/bin/env python3
import unittest
import sys
import variable

class Testvariable(unittest.TestCase):

    @unittest.skip("considering python var names for VarHost")
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
        prod.eval(values)
        prod.optimize()
        print(f"y * 2 = {prod}")
        prod_val = float(prod)
        print(f"y * 2 evaluated = {prod_val}")
        self.assertEqual(prod_val, 4.)  # 2 * 2

        print("Testing x + (y * 2)")
        expr = x + prod # 3+4=7
        print(f"Expression: {expr}")

        # Convert to float for assertion
        expr.eval(values)
        expr.optimize()
        result_float = float(expr)
        print(f"Result as float: {result_float}")
        assert result_float == 7.
        print("Test passed!")

    def test_arithmetic_operations(self):
        # Test basic operations with constants
        a = variable.Valuable(5)
        b = variable.Valuable(3)

        self.assertEqual(float(a + b), 8.0)
        self.assertEqual(float(a - b), 2.0)
        self.assertEqual(float(a * b), 15.0)
        self.assertEqual(float(a / b), 5.0/3.0)

if __name__ == "__main__":
    unittest.main()