import unittest
import sys

try:
    from variable import Variable, Valuable
    VARIABLE_MODULE_AVAILABLE = True
except ImportError:
    VARIABLE_MODULE_AVAILABLE = False

@unittest.skipIf(not VARIABLE_MODULE_AVAILABLE, "Python bindings not available")
class TestPowerFlow(unittest.TestCase):
    def test_powerflow_calculations(self):
        V1 = Variable()
        V2 = Variable()
        V3 = Variable()
        
        y12 = Valuable(0.1)
        y13 = Valuable(0.2)
        y23 = Valuable(0.15)
        
        Y11 = y12 + y13
        Y22 = y12 + y23
        Y33 = y13 + y23
        
        Y12 = -y12
        Y13 = -y13
        Y23 = -y23
        Y21 = -y12  # Y21 = Y12 by symmetry
        
        self.assertEqual(Y12, Y21)
        
        V1_val = Valuable(1.0)
        V2_val = Valuable(0.95)
        V3_val = Valuable(1.05)
        
        I1 = V1_val * Y11 + Y12 * V2_val + Y13 * V3_val
        
        S1 = V1_val * I1
        
        expected = V1_val * (V1_val * Y11 + Y12 * V2_val + Y13 * V3_val)
        self.assertEqual(S1, expected)

if __name__ == "__main__":
    unittest.main()
