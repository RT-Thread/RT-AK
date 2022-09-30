from onnx_tf.backend import prepare
import onnx
import tensorflow as tf

onnx_model = onnx.load("model_vgg6_sim.onnx")  # load onnx model
tf_rep = prepare(onnx_model)  # prepare tf representation
tf_rep.export_graph("model_vgg6_sim.tf")  # export the model
