from machine import LCD     # 从 machine 导入 LCD 类
import time
lcd = LCD()                 # 创建一个 lcd 对象
lcd.light(False)            # 关闭背光
lcd.light(True)             # 打开背光
lcd.fill(lcd.BLACK)         # 将整个 LCD 填充为黑色
time.sleep(1)
lcd.fill(lcd.RED)           # 将整个 LCD 填充为红色
time.sleep(1)
lcd.fill(lcd.GREEN)          # 将整个 LCD 填充为灰色
time.sleep(1)
lcd.fill(lcd.BLUE)         # 将整个 LCD 填充为白色
time.sleep(1)
lcd.fill(lcd.WHITE)         # 将整个 LCD 填充为白色
lcd.pixel(50, 50, lcd.BLUE) # 将（50,50）位置的像素填充为蓝色
lcd.text("hello RT-Thread", 0, 0, 16)   # 在（0, 0） 位置以 16 字号打印字符串
lcd.text("hello RT-Thread", 0, 16, 24)  # 在（0, 16）位置以 24 字号打印字符串
lcd.text("hello RT-Thread", 0, 48, 32)  # 在（0, 48）位置以 32 字号打印字符串
lcd.line(0, 50, 239, 50)    # 以起点（0，50），终点（239，50）画一条线
lcd.line(0, 50, 239, 50)    # 以起点（0，50），终点（239，50）画一条线
lcd.rectangle(100, 100, 200, 200) # 以左上角为（100,100），右下角（200,200）画矩形
lcd.circle(150, 150, 80)    # 以圆心位置（150,150），半径为 80 画圆
lcd.show_bmp(180, 50, "sun.bmp")  # 以位置（180,50）为图片左下角坐标显示文件系统中的 bmp 图片 "sun.bmp"
