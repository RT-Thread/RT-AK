#python

from k210 import sha256

str = "1234567890"
s = sha256(len(str))
s.update(str)
s.digest()

