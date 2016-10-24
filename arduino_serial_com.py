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
	print "getting lux"
	lux = ser.read(4) #lux is reported as a uint32_t (4 bytes)
	f_lux = struct.unpack('f', lux)[0]
	print "getting temp"
	temp = ser.read(4)
	f_temp = struct.unpack('f', temp)[0]
	print "getting hum"
	hum = ser.read(4)
	f_hum = struct.unpack('f', hum)[0]
	print "writing"
	with open(csv_path, 'a') as csvfile:
		w = csv.writer(csvfile)
		w.writerow([sample_id, datetime.datetime.now(), f_lux, f_temp, f_hum])

def start_mypart_sample(ser):
	# delay because it takes time to send button presses through arduino
	# and you don't want to start commanding again before the last reporting button press finishes
	time.sleep(2)
	ser.write(sample_mypart)


# output csv: 
# sample_id, datetime, device_1_small, device_1 large, device_1 humidity, device_1 temp, device_2 ...
def record_mypart_data(gzll_ser, mypart_ser, num_myparts, csv_path, sample_id):
	mypart_ser.write(send_mypart_data)
	device_data = ["nnnn"] * num_myparts
	lost = False
	retries = 1
	# Be careful about upping retries; if it takes too long trying
	# to get the data, overall timing will get thrown off
	while (retries > 0):
		for count in range(num_myparts):
			tp = try_to_read_mypart(gzll_ser)	
			if tp:
				device_data[tp[0]] = tp[1]
			else:
				lost = True
		if lost:
			retries = retries - 1
			lost = False
			print "oh no gzll lost some data...trying again..."
		else:
			break

	with open(csv_path, 'a') as csvfile:
		w = csv.writer(csvfile)
		row = [sample_id, datetime.datetime.now()]
		for d in range(0, num_myparts):
			row.extend(device_data[d])
		w.writerow(row)


def try_to_read_mypart(gzll_ser):
	
	data = gzll_ser.read(20)

	if (data):
		device_id = struct.unpack('i', data[0:4])[0]
		fu1 = struct.unpack('i', data[4:8])[0]
		fu2 = struct.unpack('i', data[8:12])[0]
		fu3 = struct.unpack('f', data[12:16])[0]
		fu4 = struct.unpack('f', data[16:20])[0]

		arr = [fu1, fu2, fu3, fu4]

		return (device_id, arr)
