import unittest
import sys

try:
    import variable
    VARIABLE_MODULE_AVAILABLE = True
except ImportError:
    VARIABLE_MODULE_AVAILABLE = False

@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
class TestCompilambda(unittest.TestCase):
    def test_compile_into_lambda(self):
        x = variable.Variable()
        y = variable.Variable()
        
        expr = x + 2 * y
        
        lambda_func = expr.compile_into_lambda([x, y])
        
        result = lambda_func([3, 4])
        self.assertEqual(float(result), 11.0)  # 3 + 2*4 = 11
        
        result = lambda_func([5, 2])
        self.assertEqual(float(result), 9.0)   # 5 + 2*2 = 9
    
    def test_compi_lambda(self):
        x = variable.Variable()
        y = variable.Variable()
        
        expr = x * y + x
        
        lambda_func = expr.compi_lambda([x, y])
        
        result = lambda_func([3, 4])
        self.assertEqual(float(result), 15.0)  # 3*4 + 3 = 15
        
        result = lambda_func([2, 5])
        self.assertEqual(float(result), 12.0)  # 2*5 + 2 = 12
    
    def test_single_variable_lambda(self):
        x = variable.Variable()
        
        expr = x * x + 1
        
        lambda_func = expr.compi_lambda([x])
        
        result = lambda_func([3])
        self.assertEqual(float(result), 10.0)  # 3*3 + 1 = 10
        
        result = lambda_func([4])
        self.assertEqual(float(result), 17.0)  # 4*4 + 1 = 17
    
    def test_multiple_variables_lambda(self):
        x = variable.Variable()
        y = variable.Variable()
        z = variable.Variable()
        w = variable.Variable()
        
        expr = x + y * z + w
        
        lambda_func = expr.compi_lambda([x, y, z, w])
        
        result = lambda_func([2, 3, 4, 5])
        self.assertEqual(float(result), 19.0)  # 2 + 3*4 + 5 = 19
        
        result = lambda_func([1, 2, 3, 4])
        self.assertEqual(float(result), 11.0)  # 1 + 2*3 + 4 = 11

if __name__ == "__main__":
    unittest.main()
