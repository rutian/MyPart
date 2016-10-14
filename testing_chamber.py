import hhpc_serial_com as hhpc
import dylos_serial_com as dylos
import arduino_serial_com as arduino
import raw_analog_saleae as raw
from Arduino import Arduino

import time
import datetime
import os

def mix(sec, comport):
	arduino.set_high(comport)
	time.sleep(sec)
	arduino.set_low(comport)

def vacuum(sec, comport):
	arduino.set_high(comport)
	time.sleep(sec)
	arduino.set_low(comport)

# use the sensors inside the box to track light and humidity
def record_ambient_readings(sample_id, csv_path, comport):
	arduino.read_from_arduino_sensors(comport, csv_path, sample_id)

def constant_mix_on(comport):
	arduino.set_high(comport)

def constant_mix_off(comport):
	arduino.set_low(comport)

def intermittent_mix_or_vacuum(vacuum_time, mix_time, internal_arduino_comport, external_arduino_comport):
	if (vacuum_time):
		print('\t turning on vacuum')
		vacuum(vacuum_time, external_arduino_comport)
		print('\t turning off vacuum')
	if (mix_time):
		print('\t turning on fans')
		mix(mix_time, internal_arduino_comport)
		print('\t turning off fans')

def start_counting(hhpc_comport, internal_arduino_comport, raw_sample_folder_path):
	hhpc.start_count(hhpc_comport) # start the count (non-blocking)
	print('\t hhpc sampling...')
	# raw.sample_and_write_analog([0,1,2], 45, raw_sample_folder_path, sample_id ) # raw analog sampling (THIS IS BLOCKING)
	arduino.start_mypart_sample(internal_arduino_comport)
	print('\t myparts sampling...')

def record_counts(dylos_comport, csv_path_dylos, hhpc_comport, csv_path_metone, csv_path_ambient, gzll_rfduino_comport, internal_arduino_comport, csv_path_mypart, sample_id):
	print('\t waiting for dylos read...')
	dylos.read_data(dylos_comport, csv_path_dylos, sample_id)
	print('\t dylos recorded')
	hhpc.get_buffer_record(hhpc_comport, csv_path_metone, sample_id) # not blocking 
	print('\t hhpc recorded')
	record_ambient_readings(sample_id, csv_path_ambient, internal_arduino_comport)
	print('\t ambient data recorded')
	arduino.record_mypart_data(gzll_rfduino_comport, internal_arduino_comport, csv_path_mypart, sample_id)
	print('\t mypart recorded')

def sleep(sleep_interval, comport):
	constant_mix_off(comport)   # turn off the fans
	arduino.toggle_servo(comport)  # turn dylos off
	time.sleep(sleep_interval)						# wait
	arduino.toggle_servo(comport)  # turn dylos on
