# coding=utf-8
'''
@ Summary: show model informations
@ Update:

@ file:    model_info.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/3/25 15:03

@ Update:  add macc & flops
            Ps: pip install keras_flops, need tf >= 2.2
@ Date:    2021/03/29
'''
import os
import argparse
from pathlib import Path
from functools import reduce
from tensorflow.keras.models import load_model
from tensorflow.python.keras.utils import layer_utils
from keras_flops.flops_calculation import get_flops

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'


class Model(object):
    def __init__(self, model):
        self.model = Path(model)
        self.model_info = list()
        self.line_length = 80

        self.check_model(self.model)

    def check_model(self, model):
        assert model.exists(),  FileNotFoundError(f"{model} not found!")

    def _get_row(self, value: list):
        return "{:20s}: {}".format(*value)

    def _cal_show_size(self, value):
        if 0 < value < 1024:
            value_size_show = value
            value_size_unit = "B"
        elif value <= 1024**2:
            value_size_show = round(value/1024, 2)
            value_size_unit = "KiB"
        else:
            value_size_show = round(value/1024/1024, 2)
            value_size_unit = "MiB"
        return value_size_show, value_size_unit

    def get_value_details(self, values, model_info, io_type="input"):
        total_values_size = 0
        for i, value in enumerate(values):
            value_items = reduce(lambda x, y: x*y, value.shape[1:])
            # real size, B, bytes
            value_size_B = value_items * 4
            # show size, KiB or Mib
            value_size_show, value_size_unit = self._cal_show_size(value_size_B)
            value_info = f"{value.op.name} [{value_items} items, {value_size_show} {value_size_unit}, " \
                         f"{value.dtype.name.upper()}, {value.shape[1:]}]"

            value_row_list = [f"{io_type}{i+1}", value_info]
            value_row_str = self._get_row(value_row_list)
            model_info.append(value_row_str)

            total_values_size += value_size_B

        total_values_size_show, total_values_size_unit = self._cal_show_size(total_values_size)
        total_values_row_list = [f"{io_type} (total)",
                                 f"{total_values_size_show} {total_values_size_unit}"]
        total_values_row_str = self._get_row(total_values_row_list)
        model_info.append(total_values_row_str)

    def get_model_info(self, ):
        # load model
        net = load_model(self.model, compile=False)

        # model summary()
        net.summary(line_length=self.line_length, print_fn=lambda x: self.model_info.append(x))
        self.model_info.append('\n')

        FLOPs, layer_flops = get_flops(net, batch_size=1)

        # add every layer FLOPs
        layer_flops_row_list = ["layer name", "FLOPs"]
        layer_flops_list = [self._get_row(layer_flops_row_list)]
        layer_flops_list.append("-"*self.line_length)
        for key, value in layer_flops.items():
            layer_flops_list.append(self._get_row([key, value]))
        layer_flops_list.append("=" * self.line_length)
        self.model_info += layer_flops_list
        self.model_info.append('\n')

        # 1. model_path
        model_path = ["model file", str(self.model.resolve())]
        model_path_str = self._get_row(model_path)
        self.model_info.append(model_path_str)

        # 2. model_name
        model_name = ["model name", self.model.stem]
        self.model_info.append(self._get_row(model_name))

        # 3. input info
        self.get_value_details(net.inputs, self.model_info)

        # 4. output info
        self.get_value_details(net.outputs, self.model_info, io_type="output")

        # 5. params
        if hasattr(net, '_collected_trainable_weights'):
            trainable_count = layer_utils.count_params(net._collected_trainable_weights)
        else:
            trainable_count = layer_utils.count_params(net.trainable_weights)

        train_count_size, train_count_unit = self._cal_show_size(trainable_count*4)
        params_row_list = ["params #",
                           f"{trainable_count} items ({train_count_size} {train_count_unit})"]
        self.model_info.append(self._get_row(params_row_list))

        # 6. macc & FLOPs
        macc_row_list = ["macc", f"{FLOPs/2}"]
        self.model_info.append(self._get_row(macc_row_list))

        flops_row_list = ["FLOPs", f"{FLOPs}"]
        self.model_info.append(self._get_row(flops_row_list))

        # 7. weights
        weights = net.count_params() * 4
        weights_size, weights_unit = self._cal_show_size(weights)
        weights_row_list = ["weights (ro)", f"{weights} B, ({weights_size} {weights_unit})"]
        self.model_info.append(self._get_row(weights_row_list))

        # 8. activations

        # 9. ram total

        # end
        self.model_info.append("="*self.line_length)

        return self.model_info

    @staticmethod
    def write_txt(file, model_info):
        f = open(file, "w+")
        f.write('\n'.join(model_info))
        f.close()

    @staticmethod
    def print_model_info(model_info):
        for line in model_info:
            print(line)


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--model", type=str, default="../Model/keras_mnist.h5",
                        help="the model path")
    parser.add_argument("--report", type=str, default="", help="the output file saved")
    parser.add_argument("--save_output", action="store_false", help="wether save output or not")
    parser.add_argument("--print_output", action="store_false", help="wether print output or not")
    opt = parser.parse_args()

    opt.report = opt.report if opt.report else f"{Path(opt.model).stem}_info.txt"

    m = Model(opt.model)
    model_info = m.get_model_info()

    if opt.print_output:
        m.print_model_info(model_info)
    if opt.save_output:
        m.write_txt(opt.report, model_info)


if __name__ == "__main__":
    main()
