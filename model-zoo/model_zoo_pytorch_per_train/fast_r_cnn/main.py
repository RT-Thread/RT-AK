import torch
from torchvision import models
from torchvision import transforms
import cv2

'''
Note: conda pytorch install opencv
windows: conda install opencv # python=3.8.8, opencv=4.0.1
ubuntu: pip3 install opencv-python # python=3.7.11, opencv=4.5.4
'''

#images_path = "../../data/image/"
#images_name = ["1.jpg", "2.jpg", "4.jpg"]
images_name = ["dog.jpg"]

images_data = []  # opencv
tensor_data = []  # pytorch tensor

for name in images_name:
    #img = cv2.imread(images_path + name)
    img = cv2.imread(name)
    #print(f"name: {images_path + name}, opencv image shape: {img.shape}")  # (w,h,c)
    print(f"name: { name}, opencv image shape: {img.shape}")  # (w,h,c)
    images_data.append(img)

    transform = transforms.Compose([transforms.ToTensor()])
    tensor = transform(img)  # Normalized Tensor image: [0., 1.]
    print(f"tensor shape: {tensor.shape}, max: {torch.max(tensor)}, min: {torch.min(tensor)}")  # (c,h,w)
    tensor_data.append(tensor)

# reference: torchvison/models/detection/faster_rcnn.py
# 使用ResNet-50-FPN(Feature Pyramid Networks, 特征金字塔网络)构建Faster RCNN模型
model = models.detection.fasterrcnn_resnet50_fpn(pretrained=True)
# print(model) # 可查看模型结构
model.eval()  # 推理
predictions = model(tensor_data)  # result: list: boxes (FloatTensor[N, 4]), labels (Int64Tensor[N]), scores (Tensor[N])
# print(predictions)

coco_labels_name = ["unlabeled", "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat",
                    "traffic light", "fire hydrant", "street sign", "stop sign", "parking meter", "bench", "bird",
                    "cat", "dog", "horse",
                    "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "hat", "backpack", "umbrella", "shoe",
                    "eye glasses", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports_ball", "kite",
                    "baseball bat",
                    "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "plate", "wine glass",
                    "cup", "fork", "knife",
                    "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot_dog", "pizza",
                    "donut", "cake", "chair", "couch", "potted plant", "bed", "mirror", "dining table", "window",
                    "desk",
                    "toilet", "door", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven",
                    "toaster", "sink", "refrigerator", "blender", "book", "clock", "vase", "scissors", "teddy bear",
                    "hair drier",
                    "toothbrush", "hair brush"]  # len = 92

for x in range(len(predictions)):
    pred = predictions[x]
    scores = pred["scores"]
    mask = scores > 0.5  # 只取scores值大于0.5的部分

    boxes = pred["boxes"][mask].int().detach().numpy()  # [x1, y1, x2, y2]
    labels = pred["labels"][mask]
    scores = scores[mask]
    print(f"prediction: boxes:{boxes}, labels:{labels}, scores:{scores}")

    img = images_data[x]

    for idx in range(len(boxes)):
        cv2.rectangle(img, (boxes[idx][0], boxes[idx][1]), (boxes[idx][2], boxes[idx][3]), (255, 0, 0))
        cv2.putText(img, coco_labels_name[labels[idx]] + " " + str(scores[idx].detach().numpy()),
                    (boxes[idx][0] + 10, boxes[idx][1] + 10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 255, 0), 1)


    cv2.imshow("image", img)
    cv2.waitKey(1000)
    cv2.imwrite("result_" + images_name[x], img)

print("test finish")
