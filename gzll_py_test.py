import serial
import csv
import datetime
import time
import struct

baud = 9600
tm = 10000


def record_mypart_data(comport, csv_path, sample_id):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		for i in range(0, 100):
			device_id = struct.unpack('i', ser.read(4))[0]
			f1 = struct.unpack('i', ser.read(4))[0]
			f2 = struct.unpack('i', ser.read(4))[0]
			f3 = struct.unpack('f', ser.read(4))[0]
			f4 = struct.unpack('f', ser.read(4))[0]
			with open(csv_path, 'a') as csvfile:
				w = csv.writer(csvfile)
				w.writerow([sample_id, datetime.datetime.now(), device_id, f1, f2, f3, f4])

def main():
	record_mypart_data('/dev/cu.usbserial-DN00CSKF', 'mypart_test5.csv', 1)


main()