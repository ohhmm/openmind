import onnx
import onnxruntime as ort

def inspect_onnx_model(model_path):
    # Load the ONNX model
    model = onnx.load(model_path)

    # Check the model's IR version
    print(f"Model IR version: {model.ir_version}")

    # Check the model's producer name and version
    print(f"Producer name: {model.producer_name}")
    print(f"Producer version: {model.producer_version}")

    # Check the model's input and output information
    print("Model inputs:")
    for input in model.graph.input:
        print(f"Name: {input.name}, Shape: {input.type.tensor_type.shape.dim}, Type: {input.type.tensor_type.elem_type}")

    print("Model outputs:")
    for output in model.graph.output:
        print(f"Name: {output.name}, Shape: {output.type.tensor_type.shape.dim}, Type: {output.type.tensor_type.elem_type}")

    # Check the model's nodes (operations)
    print("Model nodes (operations):")
    for node in model.graph.node:
        print(f"Op type: {node.op_type}, Inputs: {node.input}, Outputs: {node.output}")

if __name__ == "__main__":
    model_path = "resnet50-v1-7.onnx"
    inspect_onnx_model(model_path)
