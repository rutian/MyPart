import serial
import struct
import array
from crc_algorithms import Crc
import csv
import datetime

# -----------------------------------------
# HELPERS

def extract_4_channel(dba, i1):
	return (dba[i1] << 24) + (dba[i1 + 1] << 16) + (dba[i1 + 2] << 8) + dba[i1 + 3]

def extract_2_channel(dba, i1):
	return (dba[i1] << 8) + dba[i1 + 1]

def write_to_csv(sample_id,d1, d2, d3, d4, d5, d6, d7, d8, csvname):
	with open(csvname, 'a') as csvfile:
		w = csv.writer(csvfile)
		w.writerow([sample_id, datetime.datetime.now(), d1, d2, d3, d4, d5, d6, d7, d8])

# ------------------------------------------
# COMMANDS

def start_count(hhpc_comport, baud, tm):
	command = construct_command("01", False)
	data = send_command(command, "01", 1, hhpc_comport, baud, tm)

def stop_count(hhpc_comport, baud, tm):
    command = construct_command("02", False)
    data = send_command(command, "02", 1, hhpc_comport, baud, tm)

def get_time(hhpc_comport, baud, tm):
    command = construct_command("08", False)
    data = send_command(command, "08", 5, hhpc_comport, baud, tm)

def get_buffer_count(hhpc_comport, baud, tm):
	command = construct_command("0F", False)
	data = send_command(command, "0F", 7, hhpc_comport, baud, tm)
	bdata = bytearray(data)

	# subtract 1 to get last record
	num = (bdata[1] << 8) + bdata[2] - 1
	# turn int back into individual bytes
	bdata = struct.unpack('4B', struct.pack('>I', num))
	# it's a 4 byte int, but we want the two lsb bytes
	return [bdata[2], bdata[3]]

# use the current buffercount to get the last entry
def get_buffer_record(hhpc_comport, baud, tm, csvname, sample_id):
	recnum = get_buffer_count(hhpc_comport, baud, tm)
	command = construct_command("09", True, recnum[0], recnum[1])
	data = send_command(command, "09", 51, hhpc_comport, baud, tm)
	dba = bytearray(data)

	channel1 = extract_4_channel(dba, 1)
	channel2 = extract_4_channel(dba, 5)
	channel3 = extract_4_channel(dba, 9)
	channel4 = extract_4_channel(dba, 13)
	channel5 = extract_4_channel(dba, 17)
	channel6 = extract_4_channel(dba, 21)
	# "SECONDS SINCE 12am Jan 1 1900"
	datetime = extract_4_channel(dba, 25)
	# "Deg cel"
	temperature = extract_2_channel(dba, 29)/100
	humidity = extract_2_channel(dba, 31)

	write_to_csv(sample_id, channel1, channel2, channel3, channel4, channel5, channel6, temperature, humidity, csvname)

# ---------------------------------------------
# COMMUNICATION

def calc_crc(data):
	crc = Crc(width = 16, poly = 0x8005,
	          reflect_in = True, xor_in = 0xffff,
	          reflect_out = True, xor_out = 0x0000)

	my_crc = crc.bit_by_bit_fast(data)
	lsb = my_crc & 0b0000000011111111
	msb = (my_crc & 0b1111111100000000) >> 8
	return [lsb, msb]


def construct_command(chs, data, data1=0, data2=0):
	# construct the partial command in bytes then send to crc
	# combine with crc bytes to get full command
	prefix = 0x00

	if (data):
		crc = calc_crc(bytearray([int(chs, 16), data1, data2]))
		fullcommand = bytearray([prefix, int(chs, 16), data1, data2, crc[0], crc[1]])
	else: 
		crc = calc_crc(bytearray([int(chs, 16)]))
		fullcommand = bytearray([prefix, int(chs, 16), crc[0], crc[1]])
	return fullcommand


def send_command(command, chs, bytes_to_read, hhpc_comport, baud, tm):
	with serial.Serial(hhpc_comport, baud, timeout=tm, stopbits=serial.STOPBITS_TWO, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE) as hhpc_ser:
		num = hhpc_ser.write(command)
		line = hhpc_ser.read(bytes_to_read)
		return line




