#python

from k210 import FFT
import math

fft = FFT()

FFT_N = 64
input = []

for i in range(0, FFT_N):
v = 0.3 * math.cos(2 * math.pi * i / FFT_N + math.pi / 3) * 256
v += 0.1 * math.cos(16 * 2 * math.pi * i / FFT_N - math.pi / 9) * 256
v += 0.5 * math.cos((19 * 2 * math.pi * i / FFT_N) + math.pi / 6) * 256
input.append(int(v) + 10)



ret = fft.run(input = input, shift = 0, direction = fft.DIR_FORWARD) #DIR_BACKWARD
print(ret)

#将以上内容粘贴到MSH(注意不要删除FFT.run上的空行)
