#!/usr/bin/env python3

import os
import sys
import argparse
import platform
import serial.tools.list_ports
import subprocess

IS_WINDOWS = platform.system().upper() == "WINDOWS"
IS_LINUX = platform.system().upper() == "LINUX"
PORTS = serial.tools.list_ports.comports()
USED_PORTS = filter(lambda p: p.manufacturer is not None, PORTS)

def check_port(p: str) -> bool:
    for u in PORTS:
        if u.device == p or u.name == p:
            return True
    return False

def print_used_ports():
    print("Used ports:")
    for u in USED_PORTS:
        print(f"  {u.device} -> {u.manufacturer}")

def print_err(msg: str):
    print(f"\x1b[31mERROR: {msg}\x1b[0m", file=sys.stderr)


def upload(mcu: str, port: str, baud: int|None, programmer: str, file: str):
    command = ["avrdude", "-v"]


    if baud is not None:
        command.append("-b")
        command.append(str(baud))

    command.extend(["-p", mcu, "-c", programmer, "-P", port, "-U", "flash:w:" + file])

    try:
        print("Uploading:")
        print(f"  MCU: {mcu}")
        print(f"  Port: {port}")
        print(f"  Baud: {baud}")
        print(f"  Programmer: {programmer}")
        print(f"  File: {file}")


        result = subprocess.run(command, check=True, text=True, capture_output=True)
        print("Upload complete")
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print_err("")
        print(e.stderr, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Helper tool for uploading hex files to a microcontroller using the avrdude utility")

    parser.add_argument("--mcu", type=str, help="Microcontroller model (e.g. atmega328p)", required=True)
    parser.add_argument("--port", type=str, help="Serial port (e.g. COM3, /dev/ttyUSB0)", required=True)
    parser.add_argument("--baud", type=int, help="Baud rate", required=False)
    parser.add_argument("--programmer", type=str, help="Programmer (e.g. arduino)", required=True)
    parser.add_argument("-f", "--file", type=str, help="Hex file", required=True)

    args = parser.parse_args()

    mcu = args.mcu
    port = args.port
    baud = args.baud
    programmer = args.programmer
    file = args.file

    if IS_WINDOWS:
        os.system(" ")

    if not check_port(port):
        print_err(f"Port '{port}' not found")
        print_used_ports()
        sys.exit(1)

    if not os.path.isfile(file):
        print_err(f"File '{file}' not found")
        sys.exit(1)


    upload(mcu, port, baud, programmer, file)


