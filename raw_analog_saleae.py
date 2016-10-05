# sample some raw analog values using the saleae logic analyzer
# the logic software must be open for this to work

import os
import time
import saleae

#   folder = time.strftime('%Y-%m-%d--%H-%M-%S')
#   os.mkdir(folder)
#   
#   s = saleae.Saleae()
#   
#   s.set_active_channels(None,[0,1,2,3]) # no digital channels, all analog channels
#   s.set_sample_rate((0,125000)) # sample at 125khz (0 for the digital channel)
#   s.set_capture_seconds(6) # 60 second integration
#   
#   
#   for i in range(3):
#   	path = os.path.abspath(os.path.join(folder, str(i)))
#   	s.capture_to_file(path)
#       #s.export_data(


##### COMMANDS #####

# sample for (channels_array,seconds,path_for_saving, sample_id)
def sample_and_write_analog(channels, duration, save_folder, sample_id):
	s = saleae.Saleae()
	# set capture settings for the saleae
	s.set_sample_rate((0,125000)) # default sampling rate needed for our sensor
	s.set_active_channels(None,channels) # no digital channels, all analog channels
	s.set_capture_seconds(duration) # 60 second integration

	# file to be saved out to 
	# folder = time.strftime('%Y-%m-%d--%H-%M')
	# os.mkdir(folder) 
	path = os.path.abspath(os.path.join(save_folder, str(sample_id)))

	print path
	# capture and save, keep original and also make a matlab file
	s.capture_to_file(path)
	#s.export_data  # save also to matlab
	s.export_data2(path, None, channels, None, 'matlab') #save to matlab

# sample_and_write_analog([0,1,2,3], 2, '../automated_tests_data', 10)