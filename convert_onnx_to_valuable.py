import onnx
import onnxruntime as ort
import numpy as np

def load_onnx_model(model_path):
    """
    Load the ONNX model from the specified path.
    """
    session = ort.InferenceSession(model_path)
    return session

def get_model_inputs(session):
    """
    Get the input names and shapes of the ONNX model.
    """
    inputs = session.get_inputs()
    input_details = [(input.name, input.shape) for input in inputs]
    return input_details

def get_model_outputs(session):
    """
    Get the output names and shapes of the ONNX model.
    """
    outputs = session.get_outputs()
    output_details = [(output.name, output.shape) for output in outputs]
    return output_details

def run_inference(session, input_data):
    """
    Run inference on the ONNX model with the given input data.
    """
    input_name = session.get_inputs()[0].name
    result = session.run(None, {input_name: input_data})
    return result

def convert_to_valuable_expression(session):
    """
    Convert the ONNX model into a mathematical expression compatible with the Valuable class.
    """
    model = session.get_modelmeta().custom_metadata_map
    graph = model.graph
    valuable_expression = Valuable()

    for node in graph.node:
        if node.op_type == "Conv":
            valuable_expression.add_conv(node.input, node.output)
        elif node.op_type == "Relu":
            valuable_expression.add_relu(node.input, node.output)
        elif node.op_type == "Add":
            valuable_expression.add_add(node.input, node.output)
        # Add more operations as needed

    return valuable_expression

if __name__ == "__main__":
    model_path = "resnet50-v1-7.onnx"
    session = load_onnx_model(model_path)

    # Print model input and output details
    input_details = get_model_inputs(session)
    output_details = get_model_outputs(session)
    print("Model Inputs:", input_details)
    print("Model Outputs:", output_details)

    # Example input data (random data for demonstration purposes)
    input_data = np.random.rand(1, 3, 224, 224).astype(np.float32)

    # Run inference
    result = run_inference(session, input_data)
    print("Inference Result:", result)

    # Convert to Valuable expression
    valuable_expression = convert_to_valuable_expression(session)
    print("Valuable Expression:", valuable_expression)
