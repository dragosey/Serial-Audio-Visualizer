import serial
import time
import audioop
import sys
import math
import pyaudio


def sendToArduino(message):
    ser.write(message.encode('utf-8'))


def receiveFromArduino():
    global startChar, endChar

    msg = ""
    x = "z"  # random default character, different from < or >
    byteCount = -1

    while ord(x) != startChar:
        x = ser.read()

    while ord(x) != endChar:
        if ord(x) != startChar:
            msg = msg + x.decode('utf-8')
            byteCount += 1
        x = ser.read()

    return msg


def waitForArduino():
    global startChar, endChar

    msg = ""
    while msg.find("Arduino is ready") == -1:  # substring is not there yet
        while ser.inWaiting() == 0:
            pass

        msg = receiveFromArduino()

        print(msg)
        print()


def runTest(td):
    numLoops = len(td)
    waitingForReply = False

    n = 0
    while n < numLoops:
        teststr = td[n]

        if waitingForReply == False:
            sendToArduino(teststr)
            print("Sent from PC -- LOOP NUM " + str(n) + " TEST STR " + teststr)
            waitingForReply = True

        if waitingForReply == True:

            while ser.inWaiting() == 0:
                pass

            dataRecvd = receiveFromArduino()
            print("Reply Received  " + dataRecvd)
            n += 1
            waitingForReply = False

            print("===========")

        time.sleep(0.1)


def analyzesound():
    waitingForReply = False
    chunk = 1024  # Change if too fast/slow, never less than 1024
    scale = 50  # Change if too dim/bright
    exponent = 6  # Change if too little/too much difference between loud and quiet sounds

    # CHANGE THIS TO CORRECT INPUT DEVICE
    # Enable stereo mixing in your sound card
    # to make you sound output an input
    # Use list_devices() to list all your input devices
    device = 2

    p = pyaudio.PyAudio()
    stream = p.open(format=pyaudio.paInt16,
                    channels=1,
                    rate=44100,
                    input=True,
                    frames_per_buffer=chunk,
                    input_device_index=device)

    print("Starting, use Ctrl+C to stop")
    try:
        while True:
            data = stream.read(chunk)
            rms = audioop.rms(data, 2)

            level = min(rms / (2.0 ** 16) * scale, 1.0)
            level = level**exponent
            level = int(level * 255)
            level = str(level)
            level = str("<"+level+">")

            if waitingForReply is False:
                sendToArduino(level)
                print("Sent from PC, value send: " + level)
                #waitingForReply = True

                if waitingForReply is True:

                    while ser.inWaiting() == 0:
                        pass

                    dataRecvd = receiveFromArduino()
                    print("Reply Received  " + dataRecvd)
                    waitingForReply = False

                    print("===========")

                time.sleep(0.09)
    except KeyboardInterrupt:
        pass
    finally:
        print("\nStopping")
        stream.close()
        p.terminate()
        ser.close()


startChar = 60  # ASCII > character
endChar = 62  # ASCII > character

serialPort = "COM3"  # Make sure it's ok
baudRate = 115200  # Make sure it's ok
ser = serial.Serial(serialPort, baudRate)
print("Serial port " + serialPort + " opened Baudrate " + str(baudRate))

waitForArduino()
while True:
    analyzesound()

    if sys.exit():
        print("\nStopping")

    ser.close()


# testData = []
# testData.append("<100>")
# testData.append("<200>")
# testData.append("<100>")
# testData.append("<200>")
# testData.append("<100>")
#
# runTest(testData)

# stream.close()
# p.terminate()






