import serial
import csv
import datetime
import time
import struct

baud = 9600
tm = 10000

# these need to match the arduino code
pin_on = 'a'
pin_off = 'b'
servo_on = 'c'
read_sensors = 'd'
sample_mypart = 'e'
send_mypart_data = 'f'

num_myparts = 1

def set_manual(comport):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		while True:
			r = raw_input("a or b:")
			ser.write(pin_on)

def set_high(comport):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3) #apparently needs a delay for arduino to notice
		ser.write(pin_on)
		

def set_low(comport):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3) #apparently needs a delay for arduino to notice
		ser.write(pin_off)

def toggle_servo(comport):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3) #apparently needs a delay for arduino to notice
		ser.write(servo_on)
		time.sleep(1) #delay to keep arduino in sync

def read_from_arduino_sensors(comport, csv_path, sample_id):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3) #apparently needs a delay for arduino to notice
		ser.write(read_sensors)

		# blocks until it can read 4 bytes back from the arduino
		lux = ser.read(4) #lux is reported as a uint32_t (4 bytes)
		f_lux = struct.unpack('f', lux)[0]
		# hum = ser.read()
		hum = 0
		with open(csv_path, 'a') as csvfile:
			w = csv.writer(csvfile)
			w.writerow([sample_id, datetime.datetime.now(), f_lux, hum])

def start_mypart_sample(comport):
	with serial.Serial(comport, baud, timeout=tm) as ser:
		time.sleep(3) #apparently needs a delay for arduino to notice
		ser.write(sample_mypart)

def record_mypart_data(gzll_comport, mypart_comport, csv_path, sample_id):
	with serial.Serial(gzll_comport, baud, timeout=tm) as gzll_ser:
		with serial.Serial(mypart_comport, baud, timeout=tm) as mypart_ser:
			time.sleep(3)
			for count in range(0, num_myparts):
				mypart_ser.write(send_mypart_data)
				count_bytes = str(count).encode()
				mypart_ser.write(count_bytes)
				d_id = gzll_ser.read(4)
				device_id = struct.unpack('i', d_id)[0]
				f1 = gzll_ser.read(4)
				fu1 = struct.unpack('i', f1)[0]
				f2 = gzll_ser.read(4)
				fu2 = struct.unpack('i', f2)[0]
				f3 = gzll_ser.read(4)
				fu3 = struct.unpack('f', f3)[0]
				f4 = gzll_ser.read(4)
				fu4 = struct.unpack('f', f4)[0]
				with open(csv_path, 'a') as csvfile:
					w = csv.writer(csvfile)
					w.writerow([sample_id, datetime.datetime.now(), device_id, fu1, fu2, fu3, fu4])


# Testing:
# read_from_arduino_sensors('/dev/cu.usbmodem1431', "lighttest.csv", 1)
# toggle_servo('/dev/cu.usbmodem1431')
# set_high('/dev/cu.usbmodem1431')
# time.sleep(3)
# set_low('/dev/cu.usbmodem1431')


internal_arduino_comport = '/dev/cu.usbmodem1A1231' #fan and dylos
# gzll_rfduino_comport = '/dev/cu.usbserial-DN00CSKF' #rfduino for hosting gzll communication to myparts
start_mypart_sample(internal_arduino_comport)

# record_mypart_data(gzll_rfduino_comport, internal_arduino_comport, "/test.csv", 1)


