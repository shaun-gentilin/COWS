from time import sleep
import serial
from serial.tools import list_ports

#figure out which com port the device is associated with
#by checking available com ports before and after device is plugged in.
comport_list_pre_plugin = list(list_ports.comports())
print("Please plug in device now!")
cnt = 0
#give the user 30 seconds to plug the device in before exiting
while True:
    #check for a timeout
    if cnt >= 10:
        print('Did not detect device...')
        exit()
    #check if the available ports have changed
    comport_list_post_plugin = list(list_ports.comports())
    if len(comport_list_pre_plugin) < len(comport_list_post_plugin):
        for c in comport_list_post_plugin:
            if c not in comport_list_pre_plugin:
                comport_name = c.name
                break
        break
    cnt += 1
    sleep(1)

#print data from com port to a file
ser = serial.Serial(port=comport_name, baudrate=9600, timeout=30)
file = open(r'C:\Users\Shaun\College\Spring 2023\Capstone\test_data_file.txt', 'w')
num_data_points = 0
while(num_data_points < 100):
    data = ser.readline()
    decoded_data = data.decode()
    file.write(decoded_data)
    num_data_points += 1
file.close()
ser.close()
