# coding=utf-8

import serial
from serial.tools import list_ports
import time


class Msh():
    def __init__(self, com=None, buadrate=115200, timeout=1):
        self.buadrate = buadrate
        self.timeout = timeout

        # scan which com used
        ports_list = self.scan_ports()
        # If you have more than one coms, default choose the first one
        self.com = ports_list[0] if not com else com
        print(f"COM port now is: {self.com}")

        # create port object
        try:
            self.port = serial.Serial(self.com, 115200, timeout=timeout)
        except Exception as e:
            raise print(e)

    def __sendmsh(self, cmd):
        print("msh />", end="")
        _ = self.port.write((cmd + "\n").encode())
        b_str = self.port.readall()
        return b_str

    def scan_ports(self):
        port_info_list = list_ports.comports()
        assert port_info_list, ('not found any port')

        ports_list = [port.name for port in port_info_list]
        return ports_list

    def cmd(self, cmd='', sleepTime=0.5):
        b_str = self.__sendmsh(cmd)
        u_str = str(b_str, encoding="utf-8")
        time.sleep(sleepTime)
        print(u_str)
        return u_str

    def close(self):
        self.port.close()


if __name__ == '__main__':
    m = Msh("com4")
    m.cmd("rtak_systest")
    m.close()
