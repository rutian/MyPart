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


# Testing:
# read_from_arduino_sensors('/dev/cu.usbmodem1431', "lighttest.csv", 1)
# set_high('/dev/cu.usbmodem1431')