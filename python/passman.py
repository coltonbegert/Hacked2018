
import serial
import serial.tools.list_ports
import struct
import time
import threading

BAUD_RATE = 9600
KEY_LENGTH = 24
global Connected, Unlocked, UnlockLock, gser, Table_of_Contents, TOC_JSON
Connected = False
Unlocked = False
UnlockLock = False


Table_of_Contents = ""
TOC_JSON = None

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
    # try:
    #     by = ser.read(3)
    # except:
    #     gser = init()
    by = ser.read(3)

    # print(by)
    type, length = struct.unpack("<ch", by)
    if length > 0:

        message = ser.read(length)
        m = []
        for b in [bytes([by]) for by in message]:
            # print(b, b.decode("ascii"))
            try:
                m.append(b.decode("ascii"))
            except:
                m = []
                break
        # message = ser.read(length)
        mess = "".join(m)

    else:
        mess = None
    # print(type, message)
    return (type.decode('ascii'), mess)
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
    global Unlocked, Table_of_Contents
    if type == "K":
        print("Please Enter Master Password: ", end='')
        master_pass = input()
        if len(master_pass) < KEY_LENGTH:
            send_message(ser, "K", master_pass)
    elif type == "T":
        Table_of_Contents += message
        # print(message)
    elif type == "t":
        # print(message)

        if message is not None:
            # print("her???")
            Table_of_Contents = Table_of_Contents + message
            # print(len(Table_of_Contents), len(message))
        else:
            # print("we het here", len(Table_of_Contents))
            # TOC_JSON = "[{"
            rr = []
            for row in Table_of_Contents.split():
                r = row.split(',')
                # print(row)
                rr.append("id:'%s',website:'%s',username:'%s'" % (r[0], r[1], r[2]))
            # "},{".join(["id:'%s',username:'%s',website:'%s'"%(row[0], row[2], row[1]) for row.split(',') ])
            TOC_JSON = "[{" + ('},{').join(rr) +"}]"
            # print(TOC_JSON)
            Unlocked = True
            # print(Table_of_Contents)
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
    send_message(ser, "K", "password123")
    time.sleep(1)
   # send_message(ser, "S", "0000001")
   
    # 'P' type message specifies a password retrieval based on table of contents
    # index value
    send_message(ser, "P", "0000001")
    # print(passman_isConnected())

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

def passman_toc():
    return TOC_JSON

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
