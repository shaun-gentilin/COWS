import serial

ser = serial.Serial(port='COM6', baudrate=9600, timeout=30)
file = open(r'C:\Users\Shaun\College\Spring 2023\Capstone\test_data_file.txt', 'w')
num_data_points = 0
while(num_data_points < 100):
    data = ser.readline()
    decoded_data = data.decode()
    file.write(decoded_data)
    num_data_points += 1
file.close()
ser.close()