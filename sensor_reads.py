import testing_chamber as tc
import dylos_serial_com as dylos
import arduino_serial_com as arduino

import time
import datetime
import os

# ---------------------
# chamber setup
# ---------------------

dylos_comport = '/dev/cu.usbserial'
hhpc_comport = '/dev/cu.KeySerial1'
internal_arduino_comport = '/dev/cu.usbmodem8044' #fan, dylos, ambient chamber readings, 
external_arduino_comport = '/dev/cu.usbmodem1431' #vacuum
gzll_rfduino_comport = '/dev/cu.usbserial-DN00CSKF' #rfduino for hosting gzll communication to myparts

# ---------------------
# data saving setup
# ---------------------

csv_name = 'air_sensor_data'
parent_folder_path = '/Users/Paulosophers/Desktop/mypart/automated_tests_data/'

# ----------------------
# testing parameters
# ----------------------

cycles = 1
samples = 2
vacuum_time = 0 # seconds to run vacuum
mix_time = 5 # seconds to run fan; if you want to mix continuously during measurement, this should be -1
sleep_minutes = 0 # how many minutes you want the chamber off for in between cycles


# 
# collect and record samples from sensors in the chamber
# keep measurements synchronized on dylos 
# 
def run_test(cycles, repeat, vacuum_time, mix_time, sleep_interval, csv_path_dylos, csv_path_metone, csv_path_mypart, raw_sample_folder_path, csv_path_ambient):

	print('\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n')
	print('-------------------------------')
	print('Waiting to synchronize on Dylos')
	print('-------------------------------')

	# synchronize to the dylos
	dylos.sync_timing(dylos_comport)

	# loop through the number of cycles we want
 	for i in range(cycles):

 		if (mix_time == -1):
 			print('turning on fans')
 			tc.constant_mix_on(internal_arduino_comport)

 		# only do these if the time is > 0
 		if ((vacuum_time or mix_time) and mix_time is not -1): # time to mix, must happen in less than 60sec 
 			print('at cycle {0} of {1}: vacuum and mix'.format(i, cycles))
			tc.intermittent_mix_or_vacuum(vacuum_time, mix_time, internal_arduino_comport, external_arduino_comport)
			# stay in sync but don't record the data
			print('waiting for dylos...')
			dylos.sync_timing(dylos_comport)

		for j in range(repeat):	
			# make that terminal output look gucci
			print('-------------------------------')
			print(datetime.datetime.now())
			print('-------------------------------\n')
			

			sample_id = str(i) + "-" + str(j) 

			# time to sample, must happen in less than 60s
			print('at cycle {0}-{1} of {2}: sampling data'.format(i, j, cycles))
			tc.start_counting(hhpc_comport, internal_arduino_comport, raw_sample_folder_path)

			# read the dylos... the data taken here should be an average of the last minute (synchronized with our other reads) 
			tc.record_counts(dylos_comport, csv_path_dylos, hhpc_comport, csv_path_metone, csv_path_ambient, gzll_rfduino_comport, internal_arduino_comport, csv_path_mypart, sample_id)


		# after taking reads, turn the dylos off if a delay is desired
		# we assume the dylos starts turned on when the program is run
		if (sleep_interval):
			tc.sleep(internal_arduino_comport, sleep_interval)

	# turn dylos off for the last time!	
	if (sleep_interval):
		arduino.toggle_servo(internal_arduino_comport)  

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

	run_test(cycles, samples, vacuum_time, mix_time, sleep_interval, csv_path_dylos, csv_path_metone, csv_path_mypart, raw_sample_folder_path, csv_path_ambient)

	print('\nTest complete!')

main()
