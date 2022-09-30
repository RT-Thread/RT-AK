#python
import math
from k210 import I2S

sample_rate = 8000

i2s=I2S(0, I2S.TRANSMITTER)
i2s.init()
i2s.set_param(sample_rate, bps = 16, track_num = 1)

def sin_sound(freq, len, samrate):
pcm = bytearray()

step = (2 * math.pi * freq)/samrate

for i in range(0, len):
val = 5000 * math.sin(i * step)
iv = int(val)
pcm += iv.to_bytes(2, "little", True)


    return pcm



def play(i2s, freq, len):
snd=sin_sound(freq, len, sample_rate)
i2s.play(snd)
return



play(i2s, 523, 2048)
play(i2s, 587, 2048)
play(i2s, 659, 2048)
play(i2s, 698, 2048)
play(i2s, 784, 2048)
play(i2s, 880, 2048)
play(i2s, 988, 2048)
