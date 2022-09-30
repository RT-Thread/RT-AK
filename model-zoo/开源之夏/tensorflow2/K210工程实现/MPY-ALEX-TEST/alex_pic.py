import k210,machine,rt_ak
cam=k210.camera()
lcd=machine.LCD()
lcd.light(1)
lcd.fill(lcd.RED)
cam.reset()
cam.set_framesize(320,240)

img1,img2=cam.snapshot()
lcd.show_image(0,0,240,320,img1)
#imggry=img2.to_gray()
new_imggry=img2.resize(32,32)
new_imggry1=new_imggry.to_rgb565()
lcd.show_image(0,0,32,32,new_imggry1)
model=rt_ak.ai_find("vggnet")
rt_ak.ai_init(model)
rt_ak.ai_run(model,new_imggry)
predicts=rt_ak.ai_output(model,0)
predicts
label = ["ship","bird","automobile","cat","deer","dog","frog","horse","airplane","truck"]
index=predicts.index(max(predicts))
prenum=label[index]
prenum
lcd.text(prenum, 60, 60, 32)   # 在（0, 0） 位置以 16 字号打印字符串