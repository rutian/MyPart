import hhpc_serial_com as hhpc
import dylos_serial_com as dylos
import arduino_serial_com as arduino
import raw_analog_saleae as raw
from Arduino import Arduino

import time
import datetime
import os


dylos_comport = '/dev/cu.usbserial'
hhpc_comport = '/dev/cu.KeySerial1'
internal_arduino_comport = '/dev/cu.usbmodem1A1231' #fan and dylos
external_arduino_comport = '/dev/cu.usbmodem1431' #vacuum
gzll_rfduino_comport = '/dev/cu.usbserial-DN00CSKF' #rfduino for hosting gzll communication to myparts

csv_name = 'air_sensor_data'


#inside arduino setup
#inside_board = Arduino("9600", port="/dev/tty.usbmodem1D1321")
#fan_pin = 7
#soft_rx = 8 
#soft_tx = 9

# outside arduino stuff
# outside_board = Arduino("9600", port="/dev/tty.usbmodem1441")
# vacuum_pin = 7


# def micro_setup():
# 	# micro in the chamber
# #	inside_board.pinMode(fan_pin, "OUTPUT")
# #	inside_board.digitalWrite(fan_pin, "LOW")
# 	# inside_board.SoftwareSerial.begin(soft_rx, soft_tx, 9600)
	
# 	#micro out the chamber
# 	outside_board.pinMode(vacuum_pin, "OUTPUT")

# 	time.sleep(1)

def mix(sec):
	# inside_board.digitalWrite(fan_pin, "HIGH")
	# time.sleep(sec)
	# inside_board.digitalWrite(fan_pin, "LOW")
	# time.sleep(1) # wait, or else the fans will stay on
	
	arduino.set_high(internal_arduino_comport)
	time.sleep(sec)
	arduino.set_low(internal_arduino_comport)

def vacuum(sec):
	# outside_board.digitalWrite(vacuum_pin, "HIGH")	
	# time.sleep(sec)
	# outside_board.digitalWrite(vacuum_pin, "LOW")
  	# time.sleep(1) # must wait for serial to finish? not sure why.. otherwise the vacuum will stay on

	arduino.set_high(external_arduino_comport)
	time.sleep(sec)
	arduino.set_low(external_arduino_comport)

def sample(rounds, data_folder, raw_folder, cycle):
	csv_path = data_folder + "/" + csv_name + ".csv"
	for count in range(0, rounds+1):
		dylos.read_data(dylos_comport, csv_path)
		if (count != 0):
			hhpc.get_buffer_record(hhpc_comport, csv_path)
		hhpc.start_count(hhpc_comport)
		raw.sample_and_write_analog([0,1,2], 45, raw_folder, str(cycle) + "-" + str(count))
		count += 1
		print "NUMBER OF READS"
		print count

# use the sensors inside the box to track light and humidity
def record_ambient_readings(sample_id, csv_path):
	arduino.read_from_arduino_sensors(internal_arduino_comport, csv_path, sample_id)


# automagic sampling, synchronize every sample to the dylos serial transmissino
# less restrictions on timing than the fully synchronized sampling 
def automate_dylos_trigger(cycles, repeat):

	# the automated bits
	num_cycles = cycles
	for i in range(num_cycles):
		print('at cycle {0} of {1}'.format(i, num_cycles))
		mix(5)   # seconds
		sample(repeat, data_folder_path, raw_sample_folder_path, i) # number of samples at each particular concentration
		vacuum(5) # seconds	

	


	return 1;

# automagic sampling, but fully synchronized to the 1 minute dylos sampling
# all things (sampling/vacuum/mixing) must occur within the 1 minute window
def automate_dylos_synchronized(cycles, repeat):




	return 0;



def run_test(cycles, repeat, vacuum_time, mix_time, sleep_interval, csv_path_dylos, csv_path_metone, csv_path_mypart, raw_sample_folder_path, csv_path_ambient):

	# if they want a mixing stage, then 
	# need to run the repeats once more than 
	# just the desired number of samples
	# without a mixing stage, it's just num samples
	if (vacuum_time or mix_time):
		repeat = repeat + 1

	print('\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n')
	print('-------------------------------')
	print('Waiting to synchronize on Dylos')
	print('-------------------------------')

	# synchronize to the dylos
	dylos.read_data(dylos_comport, csv_path_dylos, '0-0')

	# loop through the number of cycles we want
 	for i in range(cycles):

		for j in range(repeat):	
			# make that terminal output look gucci
			print('-------------------------------')
			print(datetime.datetime.now())
			print('-------------------------------\n')
			

			sample_id = str(i) + "-" + str(j) 

			# only do this is they want to vaccum or mix, and it's the right loop
			if ((vacuum_time or mix_time) and (j == 0)): # time to mix, must happen in less than 60sec 
				print('at cycle {0}-{1} of {2}: vacuum and mix'.format(i, j, cycles))
				# only do these if the time is > 0
				if (vacuum_time):
					vacuum(vacuum_time)
				if (mix_time):
					mix(mix_time)

			else: # time to sample, must happen in less than 60s
				print('at cycle {0}-{1} of {2}: sampling data'.format(i, j, cycles))
				hhpc.start_count(hhpc_comport) # start the count (non-blocking)
				print('\t hhpc sampling...')
				# raw.sample_and_write_analog([0,1,2], 45, raw_sample_folder_path, sample_id ) # raw analog sampling (THIS IS BLOCKING)
				arduino.start_mypart_sample(internal_arduino_comport)
				print('\t myparts sampling...')

			# read the dylos... the data taken here should be an average of the last minute (synchronized with our other reads) 
			# must run this no matter what so we stay synchronized. toss out data later
			# if the sample id = x-0   , then that sample was taken during a vacuum/mix session
			print('waiting for dylos read')
			dylos.read_data(dylos_comport, csv_path_dylos, sample_id)
			print('dylos recorded')
			hhpc.get_buffer_record(hhpc_comport, csv_path_metone, sample_id) # not blocking 
			print('hhpc recorded')
			record_ambient_readings(sample_id, csv_path_ambient)
			print('ambient data recorded')
			arduino.record_mypart_data(gzll_rfduino_comport, internal_arduino_comport, csv_path_mypart, sample_id)
			print('mypart recorded')


		# after taking reads, turn the dylos off if a delay is desired
		# we assume the dylos starts turned on when the program is run
		if (sleep_interval):
 			arduino.toggle_servo(internal_arduino_comport)  # off
 			time.sleep(sleep_interval)						# wait
 			arduino.toggle_servo(internal_arduino_comport)  # on


def main():
	
	# set up the pins on the microcontroller
	# micro_setup()

	# save out to a folder for just the data
	data_folder_path = '/Users/Paulosophers/Desktop/mypart/automated_tests_data/' + time.strftime('%Y-%m-%d--%H-%M-%S')
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

	# sample parameters
	# ----------------------
	cycles = 40
	samples = 2
	vacuum_time = 5 # seconds
	mix_time = 40 # seconds
	sleep_minutes = .2 # minutes

	# sleep interval must be in seconds                                                                                                                                     # 
	run_test(cycles, samples, vacuum_time, mix_time, sleep_minutes * 60, csv_path_dylos, csv_path_metone, csv_path_mypart, raw_sample_folder_path, csv_path_ambient)

	if (sleep_interval):
		arduino.toggle_servo(internal_arduino_comport)  # turn if off for the last time!
	print('\nTest complete!')

main()
