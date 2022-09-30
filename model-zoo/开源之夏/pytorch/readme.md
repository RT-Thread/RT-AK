由于torch转tflite会出现不支持算子故K210工程无法部署

该文件夹下./alexnet/onnx2tflite里有pth转onnx工具，由于现在K210支持的onnx算子较少，故需要将pth->onnx->tflite再进行部署，此过程会产生较多的不支持算子，所以torch模型至今没有能够部署成功的

由于预训练模型太大，各位同学可以进入代码中执行对应指令下载或者到： [(27条消息) pytorch最全预训练模型下载与调用_Jorbol的博客-CSDN博客_pytorch预训练模型](https://blog.csdn.net/Jorbo_Li/article/details/106248808) 中下载。

数据集flower_data由于太大无法上传github，故需点击链接自行下载花分类数据集 https://storage.googleapis.com/download.tensorflow.org/example_images/flower_photos.tgz