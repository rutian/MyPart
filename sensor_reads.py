import testing_chamber as tc
import dylos_serial_com as dylos
import arduino_serial_com as arduino

import time
import datetime
import os
import serial


# ----------------------------------
# chamber setup / serial parameters
# ----------------------------------

serial_data = {
	"dylos_comport": '/dev/cu.usbserial',
	"hhpc_comport": '/dev/cu.KeySerial1',
	"internal_arduino_comport": '/dev/cu.usbmodem1D1321', #fan, dylos servo, ambient chamber readings, mypart signalling
	"external_arduino_comport": '/dev/cu.usbmodem1431', #vacuum
	"gzll_rfduino_comport": '/dev/cu.usbserial-DN00CSKF', #rfduino for hosting gzll communication to myparts
	"baud": 9600,
	"tm": 100,
	"mypart_tm": 5 #be careful about how long you make this; it can throw off overall timing if too long
}


# ---------------------
# data saving setup
# ---------------------

csv_name = 'air_sensor_data'
parent_folder_path = '/Users/Paulosophers/Desktop/mypart/automated_tests_data/'


# ----------------------
# testing parameters
# ----------------------

cycles = 60
samples = 2
vacuum_time = 0 # seconds to run vacuum in between cycles
mix_time = -1 # seconds to run fan between cycles; if you want to mix continuously during measurement, this should be -1
sleep_minutes = 0 # how many minutes you want the chamber off for in between cycles
num_myparts = 3 # how many myparts you are testing at once


# set these to true if you are including that sensor in the current test
# set to false if you are excluding that sensor, or do not have it plugged in
# note: without the dylos, all timing will be incorrect
sensor_on = {
	"dylos": True,
	"hhpc": True,
	"mypart": True,
	"saleae": False
}

# 
# collect and record samples from sensors in the chamber
# keep measurements synchronized on dylos 
# 
def run_test(sleep_interval, paths, raw_sample_folder_path):
	# open arduino serial ports once at the beginning of the test, (not HHPC and Dylos)
	# so the arduino board does not reset and lose state
	# the HHPC and Dylos require the serial port to be reopened before each command, 
	# so we will pass the comport, baud, and tm instead of a serial connection
	# and reopen serial in the individual sensor files

	with serial.Serial(serial_data["internal_arduino_comport"], serial_data["baud"], timeout=serial_data["tm"]) as internal_ser:
		# comment this in if you plug in the external arduino
		# with serial.Serial(external_arduino_comport, baud, timeout=tm) as external_ser:
			with serial.Serial(serial_data["gzll_rfduino_comport"], serial_data["baud"], timeout=serial_data["mypart_tm"]) as gzll_ser:

				serial_data["internal_ser"] = internal_ser
				serial_data["gzll_ser"] = gzll_ser

				print('\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n')
				print('-------------------------------')
				print('Waiting to synchronize on Dylos')
				print('-------------------------------')

				# synchronize to the dylos
				if sensor_on["dylos"]:
					dylos.sync_timing(serial_data["dylos_comport"], serial_data["baud"], serial_data["tm"])

				# loop through the number of cycles
			 	for i in range(cycles):

			 		if (mix_time == -1):
						tc.constant_mix_on(serial_data["internal_ser"])

			 		if ((vacuum_time > 0) or (mix_time > 0)): # time to mix, must happen in less than 60sec 
			 			print('at cycle {0} of {1}: vacuum and mix'.format(i, cycles))
						tc.intermittent_mix_or_vacuum(vacuum_time, mix_time, internal_ser, external_ser)
						
						# stay in sync but don't record the data
						print('waiting for dylos...')
						if sensor_on["dylos"]:
							dylos.sync_timing(serial_data)

					for j in range(samples):	
						# make that terminal output look gucci
						print('-------------------------------')
						print(datetime.datetime.now())
						print('-------------------------------\n')
						
						sample_id = str(i) + "-" + str(j) 

						# time to sample, must happen in less than 60s
						print('at cycle {0}-{1} of {2}: sampling data'.format(i, j, cycles))
						tc.start_counting(serial_data, paths, sensor_on, sample_id)

						# this function blocks on dylos, keeps in sync
						tc.record_counts(serial_data, paths, num_myparts, sample_id, sensor_on)

					# after taking reads, turn the dylos off if a delay is desired
					# we assume the dylos starts turned on when the program is run
					if (sleep_interval):
						tc.sleep(internal_ser, sleep_interval)

				# turn dylos off for the last time!	
				if (sleep_interval):
					arduino.toggle_servo(internal_ser) 

				# make sure fans are off 
				tc.constant_mix_off(internal_ser)

def main():
	# save out to a folder for just the data
	data_folder_path = parent_folder_path + time.strftime('%Y-%m-%d--%H-%M-%S')
	if not os.path.exists(data_folder_path):
		os.makedirs(data_folder_path)

	# put in an about.txt for info about the text
	text_file = open(data_folder_path + "/about.txt", "w")
	text_file.write("Notes about this particular test here...")
	text_file.close()

	# folder for the raw analog samples
	raw_sample_folder_path = data_folder_path + "/raw_samples"
	if not os.path.exists(raw_sample_folder_path):
		os.makedirs(raw_sample_folder_path)

	# data csvs
	csv_path_metone = data_folder_path + "/" + csv_name + "_metone.csv"
	csv_path_dylos = data_folder_path + "/" + csv_name + "_dylos.csv"
	csv_path_mypart = data_folder_path + "/" + csv_name + "_mypart.csv"
	csv_path_ambient = data_folder_path + "/" + csv_name + "_ambient.csv"

	paths = {
		"dylos": csv_path_dylos, 
		"hhpc": csv_path_metone, 
		"mypart": csv_path_mypart, 
		"ambient": csv_path_ambient, 
		"raw": raw_sample_folder_path
	}

	sleep_interval = sleep_minutes * 60  # sleep interval must be in seconds                                                                                                                         

	run_test(sleep_interval, paths, raw_sample_folder_path)

	print('\nTest complete!')

main()

