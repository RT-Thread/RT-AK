from k210 import FPIOA
from machine import PWM

fpioa = FPIOA() 
# 查看fpio的方法和可选功能映射
dir(fpioa)
# TIMER2_TOGGLE1-4对应PWM0-3通道
# 设置30号IO引脚(LED)功能映射为TIMER2_TOGGLE1(PWM通道0), 
fpioa.set_function(30, fpioa.TIMER2_TOGGLE1)
pwm=PWM('pwm',0,1000,250)
pwm.init(0,1000,150)
pwm.init(0,1000,0)   