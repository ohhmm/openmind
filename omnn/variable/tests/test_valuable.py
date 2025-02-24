import valuable

def test_basic_arithmetic():
    try:
        print("Creating variable x")
        x = valuable.Variable("x")
        print("Creating variable y")
        y = valuable.Variable("y")
        
        print("Setting x to 3")
        x.set_value(3)
        print("Setting y to 4")
        y.set_value(4)
        
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
