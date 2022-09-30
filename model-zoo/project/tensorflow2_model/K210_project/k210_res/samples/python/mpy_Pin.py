from machine import Pin
# 查看Pin类成员
dir(Pin)
# machine.Pin( id, mode = -1, pull = -1，value)
# id: ('name', num) name为用户引脚别名, num为引脚编号(和原理图IO0-IO47对应).
pin30 = Pin(('rgb_r',30), mode=Pin.OUT_PP, value=1)
pin38 = Pin(('rgb_r',38), mode=Pin.OUT_PP, value=1)
pin39 = Pin(('rgb_g',39), mode=Pin.OUT_PP, value=1)
pin40 = Pin(('rgb_b',40), mode=Pin.OUT_PP, value=1)
# light led
pin30.value(0)
pin38.value(0)
pin39.value(0)
pin40.value(0)
