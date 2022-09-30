#python

from k210 import I2S

i2sp = I2S(0, I2S.TRANSMITTER)
i2sp.init()
i2sp.set_param(8000, bps = 16, track_num = 2)

i2sr = I2S(1, I2S.RECEIVER)
i2sr.init()
i2sr.set_param(8000)

while (True):
pcm = i2sr.record()
i2sp.play(pcm)




#
