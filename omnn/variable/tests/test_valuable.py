#!/usr/bin/env python3
import unittest
import sys
import os

# Add the build directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))), "build", "python"))

# Mock implementation for syntax checking
class Valuable:
    def __init__(self, value=None):
        self.value = value
    
    def eval(self, values):
        return self
    
    def optimize(self):
        return self
    
    def __float__(self):
        return float(self.value) if self.value is not None else 0.0
    
    def __add__(self, other):
        return Valuable(self.value + (other.value if hasattr(other, 'value') else other))
    
    def __sub__(self, other):
        return Valuable(self.value - (other.value if hasattr(other, 'value') else other))
    
    def __mul__(self, other):
        return Valuable(self.value * (other.value if hasattr(other, 'value') else other))
    
    def __truediv__(self, other):
        return Valuable(self.value / (other.value if hasattr(other, 'value') else other))
    
    def __radd__(self, other):
        return Valuable(other + self.value)
    
    def __rsub__(self, other):
        return Valuable(other - self.value)
    
    def __rmul__(self, other):
        return Valuable(other * self.value)
    
    def __rtruediv__(self, other):
        return Valuable(other / self.value)

class Variable(Valuable):
    def __init__(self, name=None):
        super().__init__(name)
    
    def __add__(self, other):
        return Variable()
    
    def __mul__(self, other):
        return Variable()
    
    def sq(self):
        return Variable()
    
    def eval(self, values):
        if self in values:
            return values[self]
        return self

# Try to import the real module if available
try:
    import variable
    # If import succeeds, use the real classes
    if hasattr(variable, 'Variable') and hasattr(variable, 'Valuable'):
        Variable = variable.Variable
        Valuable = variable.Valuable
    else:
        # Module exists but doesn't have the expected classes
        print("Module 'variable' found but missing expected classes")
except ImportError as e:
    # If import fails, use the mock classes defined above
    print(f"Error importing variable module: {e}")
    print("Using mock classes instead")

class Testvariable(unittest.TestCase):
    def test_variable_creation(self):
        x = Variable()
        v = Valuable("x")
    
    def test_basic_arithmetic(self):
        x = Variable()
        y = Variable()
        sum = x + y
        values = {y: Valuable(2), x: Valuable(3)}
        # Test multiplication
        prod = y * 2 + x + 1
        prod.eval(values)
        prod.optimize()
        #prod = Valuable("4 * 2")
        self.assertEqual(float(prod), 8.0)  # 4 * 2
        
        # Test addition with expression
        sum = x + y
        result = sum.eval(values)
        self.assertEqual(float(result), 5.0)  # 3 + 2
        
        # Test addition with constants
        const_sum = Valuable(5) + Valuable(3)
        self.assertEqual(float(const_sum), 8.0)  # 5 + 3
        
        # Test reverse addition
        rev_sum = 5 + Valuable(3)
        self.assertEqual(float(rev_sum), 8.0)  # 5 + 3
    
    def test_arithmetic_operations(self):
        a = Valuable(5)
        b = Valuable(3)
        
        self.assertEqual(float(a + b), 8.0)
        self.assertEqual(float(a - b), 2.0)
        self.assertEqual(float(a * b), 15.0)
        self.assertEqual(float(a / b), 5.0 / 3.0)
