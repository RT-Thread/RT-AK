# coding=utf-8
'''
@ Summary: wget and unzip
@ Update:  

@ file:    download_file.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/3/16 20:30
'''
import os
import wget
import zipfile
from pathlib import Path

def download_unzip(urls, target):
    target = Path(target)
    print("Start download files...")
    if not target.exists():
        target.mkdir(parents=True)

    for i in range(len(urls)):
        download_file = Path(urls[i]).name
        wget.download(urls[i], str(target))
        zfile = zipfile.ZipFile(target/download_file)
        zfile.extractall(target)
        print(f"\n{download_file} unzip successfuly...")
    print("\nEnd download files...")


def pre_bsp_gnu_exttools(tmp="../tmp"):
    urls = ["http://117.143.63.254:9012/www/RT-AK/sdk-bsp-artpi.zip",
            "http://117.143.63.254:9012/www/RT-AK/mingw.zip",
            "http://117.143.63.254:9012/www/RT-AK/stm32ai-windows-5.2.0.zip"]
    _ = download_unzip(urls, tmp)
    os.environ["GNU"] = str((Path(tmp)/"mingw/bin").resolve())
    os.environ["BSP"] = str((Path(tmp)/"sdk-bsp-artpi").resolve())
    os.environ["EXT_TOOLS"] = str((Path(tmp)/"windows").resolve())


if __name__ == "__main__":
    pre_bsp_gnu_exttools()
    print(os.getenv("BSP"))
