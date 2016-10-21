import testing_chamber as tc
import dylos_serial_com as dylos
import arduino_serial_com as arduino

import time
import datetime
import os
import serial


# ---------------------
# chamber setup
# ---------------------

dylos_comport = '/dev/cu.usbserial'
hhpc_comport = '/dev/cu.KeySerial1'
internal_arduino_comport = '/dev/cu.usbmodem8050' #fan, dylos servo, ambient chamber readings, mypart signalling
external_arduino_comport = '/dev/cu.usbmodem1431' #vacuum
gzll_rfduino_comport = '/dev/cu.usbserial-DN00CSKF' #rfduino for hosting gzll communication to myparts

# ---------------------
# data saving setup
# ---------------------

csv_name = 'air_sensor_data'
parent_folder_path = '/Users/Paulosophers/Desktop/mypart/automated_tests_data/'

# ----------------------
# serial parameters
# ----------------------

baud = 9600
tm = 10000

# ----------------------
# testing parameters
# ----------------------

cycles = 60
samples = 2
vacuum_time = 0 # seconds to run vacuum in between cycles
mix_time = -1 # seconds to run fan between cycles; if you want to mix continuously during measurement, this should be -1
sleep_minutes = 0 # how many minutes you want the chamber off for in between cycles
num_myparts = 0 # how many myparts you are testing at once


# 
# collect and record samples from sensors in the chamber
# keep measurements synchronized on dylos 
# 
def run_test(sleep_interval, csv_path_dylos, csv_path_metone, csv_path_mypart, csv_path_ambient, raw_sample_folder_path):
	# open arduino serial ports once at the beginning of the test, (not HHPC and Dylos)
	# so the arduino board does not reset and lose state
	# the HHPC and Dylos require the serial port to be reopened before each command, 
	# so we will pass the comport, baud, and tm instead of a serial connection
	# and reopen serial in the individual sensor files

	with serial.Serial(internal_arduino_comport, baud, timeout=tm) as internal_ser:
		# comment this in if you plug in the external arduino
		# with serial.Serial(external_arduino_comport, baud, timeout=tm) as external_ser:
			with serial.Serial(gzll_rfduino_comport, baud, timeout=tm) as gzll_ser:

				print('\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n')
				print('-------------------------------')
				print('Waiting to synchronize on Dylos')
				print('-------------------------------')

				# synchronize to the dylos
				dylos.sync_timing(dylos_comport, baud, tm)

				# loop through the number of cycles
			 	for i in range(cycles):

			 		if (mix_time == -1):
						tc.constant_mix_on(internal_ser)

			 		if ((vacuum_time > 0) or (mix_time > 0)): # time to mix, must happen in less than 60sec 
			 			print('at cycle {0} of {1}: vacuum and mix'.format(i, cycles))
						tc.intermittent_mix_or_vacuum(vacuum_time, mix_time, internal_ser, external_ser)
						
						# stay in sync but don't record the data
						print('waiting for dylos...')
						dylos.sync_timing(dylos_comport, baud, tm)

					for j in range(samples):	
						# make that terminal output look gucci
						print('-------------------------------')
						print(datetime.datetime.now())
						print('-------------------------------\n')
						
						sample_id = str(i) + "-" + str(j) 

						# time to sample, must happen in less than 60s
						print('at cycle {0}-{1} of {2}: sampling data'.format(i, j, cycles))
						# tc.start_counting(hhpc_comport, baud, tm, internal_ser, raw_sample_folder_path)

						# this function blocks on dylos, keeps in sync
						tc.record_counts(dylos_comport, csv_path_dylos, hhpc_comport, baud, tm, csv_path_metone, csv_path_ambient, gzll_ser, internal_ser, num_myparts, csv_path_mypart, sample_id)

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

	sleep_interval = sleep_minutes * 60  # sleep interval must be in seconds                                                                                                                         

	run_test(sleep_interval, csv_path_dylos, csv_path_metone, csv_path_mypart, csv_path_ambient, raw_sample_folder_path)

	print('\nTest complete!')

main()

