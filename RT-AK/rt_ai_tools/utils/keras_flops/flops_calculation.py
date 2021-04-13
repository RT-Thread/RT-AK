from typing import Optional, Union
import tensorflow as tf
from tensorflow.python.framework.convert_to_constants import (
    convert_variables_to_constants_v2_as_graph,
)

from tensorflow.keras import Sequential, Model


def get_flops(model: Union[Model, Sequential], batch_size: Optional[int] = 1) -> tuple:
    """
    Calculate FLOPS for tf.keras.Model or tf.keras.Sequential .
    Ignore operations used in only training mode such as Initialization.
    Use tf.profiler of tensorflow v1 api.
    """
    assert isinstance(model, (Sequential, Model)), \
        KeyError("model arguments must be tf.keras.Model or tf.keras.Sequential instanse")

    # convert tf.keras model into frozen graph to count FLOPS about operations used at inference
    # FLOPS depends on batch size
    inputs = [
        tf.TensorSpec([batch_size] + inp.shape[1:], inp.dtype) for inp in model.inputs
    ]
    real_model = tf.function(model).get_concrete_function(inputs)
    frozen_func, frozen_graph = convert_variables_to_constants_v2_as_graph(real_model)

    # Calculate FLOPS with tf.profiler
    run_meta = tf.compat.v1.RunMetadata()
    opts = tf.compat.v1.profiler.ProfileOptionBuilder.float_operation()
    flops = tf.compat.v1.profiler.profile(
        graph=frozen_func.graph, run_meta=run_meta, cmd="scope", options=opts
    )
    # print(frozen_func.graph.get_operations())
    # TODO: show each FLOPS
    layers_dict = dict()
    for layer in flops.children:
        name = layer.name.split("/")[1:]
        name = "/".join(name)
        layers_dict[name] = layer.total_float_ops
    return (flops.total_float_ops, layers_dict)


if __name__ == "__main__":
    from tensorflow.keras.models import load_model

    mnist = "../../Model/keras_mnist.h5"
    model = load_model(mnist)
    FLOPs, layers_dict = get_flops(model)
    print(f"FLOPs: {FLOPs}")
    for key, value in layers_dict.items():
        print(f"{key}: {value}")
