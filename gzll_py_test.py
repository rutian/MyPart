import serial
import csv
import datetime
import time
import struct

baud = 9600
tm = 10000


def record_mypart_data(comport, csv_path, sample_id):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3)
		d_id = ser.read(4)
		device_id = struct.unpack('i', d_id)[0]
		f1 = ser.read(4)
		fu1 = struct.unpack('i', f1)[0]
		f2 = ser.read(4)
		fu2 = struct.unpack('i', f2)[0]
		f3 = ser.read(4)
		fu3 = struct.unpack('f', f3)[0]
		f4 = ser.read(4)
		fu4 = struct.unpack('f', f4)[0]
		with open(csv_path, 'a') as csvfile:
			w = csv.writer(csvfile)
			w.writerow([sample_id, datetime.datetime.now(), device_id, fu1, fu2, fu3, fu4])

# def test():
# 	while True:
# 		record_mypart_data('/dev/cu.usbserial-DN00CSKF', 'mypart_test.csv', 1)


# test()