import numpy as np
from tensorflow.python.framework import ops
from tensorflow.python.framework import graph_util
from tensorflow.python.profiler.internal.flops_registry import (
    _reduction_op_flops,
    _binary_per_element_op_flops,
)


@ops.RegisterStatistics("FusedBatchNormV3", "flops")
def _flops_fused_batch_norm_v3(graph, node):
    """inference is only supportted"""
    in_shape = graph_util.tensor_shape_from_node_def_name(graph, node.input[0])
    in_shape.assert_is_fully_defined()
    mean_shape = graph_util.tensor_shape_from_node_def_name(graph, node.input[3])
    mean_shape.assert_is_fully_defined()
    variance_shape = graph_util.tensor_shape_from_node_def_name(graph, node.input[4])
    variance_shape.assert_is_fully_defined()

    if node.attr["is_training"].b is True:
        raise ValueError("Only supports inference mode")

    num_flops = (
        2 * in_shape.num_elements()
        + 5 * variance_shape.num_elements()
        + mean_shape.num_elements()
    )
    return ops.OpStats("flops", num_flops)


@ops.RegisterStatistics("Max", "flops")
def _flops_max(graph, node):
    """inference is supportted"""
    # reduction - comparison, no finalization
    return _reduction_op_flops(graph, node, reduce_flops=1, finalize_flops=0)


@ops.RegisterStatistics("AddV2", "flops")
def _flops_add(graph, node):
    """inference is supportted"""
    return _binary_per_element_op_flops(graph, node)

