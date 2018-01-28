
import serial
import serial.tools.list_ports
import struct
import time
import threading

BAUD_RATE = 9600
KEY_LENGTH = 24
global Connected, Unlocked, UnlockLock, gser
Connected = False
Unlocked = False
UnlockLock = False


Table_of_Contents = ""

def init():
    global Connected
    port = None
    while port is None:
        devices = serial.tools.list_ports.comports()
        for device in devices:
            # print(device.name, device.location, device.device, device.description)
            if device.description == "Feather 32u4" or device.description == "Adafruit Feather":
                print("Found Passman device on", device.device)
                port = device.device
            # else:
            #     print("no device found ....  Trying again")
            #     time.sleep(1)
        if port is None:
            print("no device found ....  Trying again")
            time.sleep(1)

    # if port is None:
    #     i = 0
    #     dev = []
    #     print ("Select Device")
    #     for device in devices:
    #         print(i, device.device, device.description, device.name)
    #         dev.append(device.device)
    #         i += 1
    #     port = dev[int(input())]

    ser = serial.Serial(
        port=port,
        baudrate=BAUD_RATE,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )
    
    if ser.is_open:
        Connected = True
        return ser
    print("Serial failed to init")
    exit(1)


def get_message(ser):
    global gser
    # print("hello")
    # type = ser.read(1)
    # for i in range(3):
    #     print(ser.read(1))
    try:
        by = ser.read(3)
    except:
        gser = init()

    # print(by)
    type, length = struct.unpack("<ch", by)
    if length > 0:
        message = ser.read(length).decode("ascii")
    else:
        message = None
    # print(type, message)
    return (type.decode('ascii'), message)
    # if type.decode("ascii") == "m":
    #     # length = int.from_bytes(ser.read(2), byteorder='big')
    #     print(type, length)
    #     print(ser.read(length))
        
    # else:
    #     print("gross:", str(type))

def send_message(ser, type, message):
    # print(message.encode())
    # s = bytes(message, "ascii")
    # print(s)
    struct_fmt = "!ch{}s".format(len(message))
    string = struct.pack(struct_fmt, type.encode(), len(message), message.encode())
    # time.sleep(1)
    ser.write(string)
    print(string)

def handle_message(ser, type, message):
    global UnlockLock
    if type == "K":
        print("Please Enter Master Password: ", end='')
        master_pass = input()
        if len(master_pass) < KEY_LENGTH:
            send_message(ser, "K", master_pass)
    elif type == "T":
        Table_of_Contents = message
    elif type == "t":
        if message is not None:
            Table_of_Contents += message
        else:
            UnlockLock = False
    elif type == "D":
        print("ADA ECHOED:", message)

def passman_read(ser):
    while True:
        message_type, message = get_message(ser)
        handle_message(ser, message_type, message)

def passman_create():
    global gser
    ser = init()
    gser = ser
    t1 = threading.Thread(target=passman_read, args=(ser,))
    t1.start()
    print(passman_isConnected())

    # while True:
    #     time.sleep(15)
    #     send_message(ser, "m", "keepalive")

def passman_isConnected():
    global Connected
    return Connected


def passman_isUnlocked():
    global Unlocked
    return Unlocked

def passman_attemptUnlock(password):
    global UnlockLock, gser, Unlocked
    # UnlockLock = True
    send_message(gser, "K", password)
    print("attempted unclok")
    # Unlocked = True

    # while UnlockLock:
    #     time.sleep(0.1)


def main():
    passman_create()
    # ser = init()
    # # send_message(ser, "m", "I can type any message")
    # # while True:
    # #     print(ser.read(1))
    # #     # print("aa")


    # while True:
    #     # send_message(ser, "m", "test")
    #     message_type, message = get_message(ser)
    #     handle_message(ser, message_type, message)
    #     time.sleep(1)
    #     # print(ser.read())



if __name__ == "__main__":
    main()
