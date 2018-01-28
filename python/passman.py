import serial
import serial.tools.list_ports
import struct
import time

BAUD_RATE = 9600

def init():
    devices = serial.tools.list_ports.comports()

    port = None
    for device in devices:
        # print(device.name, device.location, device.device, device.description)
        if device.description == "Feather 32u4" or device.description == "Adafruit Feather":
            print("Found Passman device on", device.device)
            port = device.device

    if port is None:
        i = 0
        dev = []
        print ("Select Device")
        for device in devices:
            print(i, device.device, device.description, device.name)
            dev.append(device.device)
            i += 1
        port = dev[int(input())]

    ser = serial.Serial(
        port=port,
        baudrate=BAUD_RATE,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )
    
    if ser.is_open:
        return ser
    print("Serial failed to init")
    exit(1)


def get_message(ser):
    # print("hello")
    # type = ser.read(1)
    # for i in range(3):
    #     print(ser.read(1))
    by = ser.read(3)
    # print(by)
    type, length = struct.unpack("<ch", by)
    if type.decode("ascii") == "m":
        # length = int.from_bytes(ser.read(2), byteorder='big')
        print(type, length)
        print(ser.read(length))
    else:
        print("gross:", str(type))

def send_message(ser, type, message):
    # print(message.encode())
    # s = bytes(message, "ascii")
    # print(s)
    struct_fmt = "!ch{}s".format(len(message))
    string = struct.pack(struct_fmt, type.encode(), len(message), message.encode())
    # time.sleep(1)
    ser.write(string)
    print(string)


def main():
    
    ser = init()
    # send_message(ser, "m", "I can type any message")
    # while True:
    #     print(ser.read(1))
    #     # print("aa")


    while True:
        send_message(ser, "m", "test")
        get_message(ser)
        # print(ser.read())



if __name__ == "__main__":
    main()
