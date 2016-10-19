import serial
import csv
import datetime
import time
import struct

# these need to match the arduino code
pin_on = 'a'
pin_off = 'b'
servo_on = 'c'
read_sensors = 'd'
sample_mypart = 'e'
send_mypart_data = 'f'

def set_manual(ser):
	while True:
		r = raw_input("a or b:")
		ser.write(pin_on)

def set_high(ser):
	ser.write(pin_on)
		
def set_low(ser):
	ser.write(pin_off)

def toggle_servo(ser):
	ser.write(servo_on)
	time.sleep(1) #delay to keep arduino in sync


# reports <sample id, date, light, temperature, humidity>
def read_from_arduino_sensors(ser, csv_path, sample_id):
	ser.write(read_sensors)

	# blocks until it can read 4 bytes back from the arduino
	lux = ser.read(4) #lux is reported as a uint32_t (4 bytes)
	f_lux = struct.unpack('f', lux)[0]
	temp = ser.read(4)
	f_temp = struct.unpack('f', temp)[0]
	hum = ser.read(4)
	f_hum = struct.unpack('f', hum)[0]
	with open(csv_path, 'a') as csvfile:
		w = csv.writer(csvfile)
		w.writerow([sample_id, datetime.datetime.now(), f_lux, f_temp, f_hum])

def start_mypart_sample(ser):
	# delay because it takes time to send button presses through arduino
	# and you don't want to start commanding again before the last reporting button press finishes
	time.sleep(3)
	ser.write(sample_mypart)


# output csv: 
# sample_id, datetime, device_1_small, device_1 large, device_1 humidity, device_1 temp, device_2 ...
def record_mypart_data(gzll_ser, mypart_ser, num_myparts, csv_path, sample_id):
	mypart_ser.write(send_mypart_data)
	device_data = [None] * num_myparts
	for count in range(0, num_myparts):			
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
		device_data[device_id] = [fu1, fu2, fu3, fu4]
	with open(csv_path, 'a') as csvfile:
		w = csv.writer(csvfile)
		row = [sample_id, datetime.datetime.now()]
		for d in range(0, num_myparts):
			row.extend(device_data[d])
		w.writerow(row)
