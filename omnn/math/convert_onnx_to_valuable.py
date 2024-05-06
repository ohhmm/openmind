import onnx
import onnxruntime as ort
from omnn.math import Valuable

def convert_onnx_to_valuable(model_path):
    # Load the ONNX model
    model = onnx.load(model_path)

    # Initialize the ONNX Runtime environment
    env = ort.Env(ort.ORT_LOGGING_LEVEL_WARNING, "ValuableONNX")

    # Create an ONNX Runtime session options object
    session_options = ort.SessionOptions()
    session_options.SetIntraOpNumThreads(1)
    session_options.SetGraphOptimizationLevel(ort.GraphOptimizationLevel.ORT_ENABLE_EXTENDED)

    # Load the ONNX model into an ONNX Runtime session
    session = ort.InferenceSession(model_path, session_options)

    # Parse the ONNX model and convert it into a Valuable object
    valuable_model = Valuable()

    # Iterate through the nodes in the ONNX model's computational graph
    for node in model.graph.node:
        op_type = node.op_type
        inputs = node.input
        outputs = node.output

        # Convert each node into a mathematical expression and add it to the Valuable object
        if op_type == "Conv":
            # Example: Convert convolution operation
            valuable_model.add_conv(inputs, outputs)
        elif op_type == "Relu":
            # Example: Convert ReLU activation function
            valuable_model.add_relu(inputs, outputs)
        elif op_type == "Add":
            # Example: Convert addition operation
            valuable_model.add_add(inputs, outputs)
        # Add more operations as needed

    return valuable_model

if __name__ == "__main__":
    model_path = "resnet50-v1-7.onnx"
    valuable_model = convert_onnx_to_valuable(model_path)
    print("Converted ONNX model to Valuable object.")
