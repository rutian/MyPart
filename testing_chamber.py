import hhpc_serial_com as hhpc
import dylos_serial_com as dylos
import arduino_serial_com as arduino
import raw_analog_saleae as raw

import time
import datetime
import os

# turn on fans for a certain amount of time, then turn them off
def mix(sec, ser):
	arduino.set_high(ser)
	time.sleep(sec)
	arduino.set_low(ser)

# turn on vacuum for a certain amount of time, then turn it off
def vacuum(sec, ser):
	arduino.set_high(ser)
	time.sleep(sec)
	arduino.set_low(ser)

# use the sensors inside the box to track light and humidity
def record_ambient_readings(sample_id, csv_path, ser):
	arduino.read_from_arduino_sensors(ser, csv_path, sample_id)

# turn on fans and leave them on
def constant_mix_on(ser):
	print('\t turning on fans')
	arduino.set_high(ser)

# turn off fans and leave them off
def constant_mix_off(ser):
	print('\t turning off fans')
	arduino.set_low(ser)

# control vacuum and fans in between cycles
def intermittent_mix_or_vacuum(vacuum_time, mix_time, internal_ser, external_ser):
	if (vacuum_time > 0):
		print('\t turning on vacuum')
		vacuum(vacuum_time, external_ser)
		print('\t turning off vacuum')
	if (mix_time > 0):
		print('\t turning on fans')
		mix(mix_time, internal_ser)
		print('\t turning off fans')

# start sampling for any sensor that requires an explicit command (hhpc, mypart)
# include the saleae if you want raw readings from the mypart -- note that the saleae is blocking
# the dylos is not here since it samples continuously
def start_counting(serial_data, paths, sensor_on, sample_id):
	if sensor_on["hhpc"]:
		hhpc.start_count(serial_data["hhpc_comport"], serial_data["baud"], serial_data["tm"]) # start the count (non-blocking)
		print('\t hhpc sampling...')
	if sensor_on["mypart"]:
		arduino.start_mypart_sample(serial_data["internal_ser"])
		print('\t myparts sampling...')
	if sensor_on["saleae"]:
		print('\t saleae sampling...')
		raw.sample_and_write_analog([0,1,2], 35, paths["raw"], sample_id) # raw analog sampling (THIS IS BLOCKING)
		print('\t saleae finished')

# send request for data from each sensor and record results to csvs
def record_counts(serial_data, paths, num_myparts, sample_id, sensor_on):
	if sensor_on["dylos"]:
		print('\t waiting for dylos read...')
		# read the dylos... the data taken here should be an average of the last minute (synchronized with our other reads) 
		dylos.read_data(serial_data["dylos_comport"], serial_data["baud"], serial_data["tm"], paths["dylos"], sample_id)
		print(datetime.datetime.now())
		print('\t dylos recorded')
	if sensor_on["hhpc"]:
		hhpc.get_buffer_record(serial_data["hhpc_comport"], serial_data["baud"], serial_data["tm"], paths["hhpc"], sample_id) # not blocking 
		print('\t hhpc recorded')

	record_ambient_readings(sample_id, paths["ambient"], serial_data["internal_ser"])
	print('\t ambient data recorded')

	if sensor_on["mypart"]:
		arduino.record_mypart_data(serial_data["gzll_ser"], serial_data["internal_ser"], num_myparts, paths["mypart"], sample_id)
		print('\t mypart recorded')
		time.sleep(10)

# turn off fans and dylos in between cycles
# and block execution for given sleep interval
# note that fan-on is controlled at the top of the cycle, not here
def sleep(sleep_interval, ser):
	constant_mix_off(ser)   # turn off the fans
	arduino.toggle_servo(ser)  # turn dylos off
	time.sleep(sleep_interval)						# wait
	arduino.toggle_servo(ser)  # turn dylos on


