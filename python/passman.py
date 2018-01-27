import serial
import serial.tools.list_ports

BAUD_RATE = 9600

def init():
    devices = serial.tools.list_ports.comports()

    port = None
    for device in devices:
        # print(device.name, device.location, device.device, device.description)
        if device.description == "Feather 32u4":
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

def main():
    ser = init()

    while True:
        print(ser.read())



if __name__ == "__main__":
    main()
