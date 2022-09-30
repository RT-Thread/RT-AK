#python
from k210 import FPIOA

fpioa = FPIOA()
fpioa.set_function(1, fpioa.JTAG_TDI)

pin = fpioa.get_io_by_function(fpioa.I2C0_SCLK) #pin is assigned by driver
fpioa.set_function(pin, fpioa.I2C1_SCLK) #return false
