import serial
import csv
import datetime
import time

baud = 9600
tm = 10000

# these need to match the arduino code
pin_on = 'a'
pin_off = 'b'
servo_on = 'c'

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
