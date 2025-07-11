import sys

sys.path.append('./bin')

from yoyo.genesis.utils import *

##################################################
################## FILE_EXISTS ###################

# FILE_EXISTS-1
if is_file( "./../../data/table.csv" ):
	print("Error at FILE_EXISTS-1")

# FILE_EXISTS-2
if is_file( "/road/to/nowhere" ):
	print("Error at FILE_EXISTS-2")

# FILE_EXISTS-3
if is_file( "./../../data/" ):
	print("Error at FILE_EXISTS-3")

##################################################
################## CURRENT_PATH ##################

# CURRENT_PATH-1
try:
	current_path()
except:
	print("Error at CURRENT_PATH-1")

# CURRENT_PATH-2
cwd = current_path()
if 0 == len(cwd):
	print("Error at CURRENT_PATH-2")

##################################################
################### REAL_PATH ####################

# REAL_PATH-1
cwd = current_path()
try:
	real_path(cwd)
except:
	print("Error at REAL_PATH-1")

# REAL_PATH-2
rp = real_path(cwd)
if 0 == len(rp):
	print("Error at REAL_PATH-2")

##################################################
################### READLINES ####################

infile = "./test/data/table.csv"
lines = file_read_lines( infile )

if 11 != len(lines):
	print("Error at READLINES-1")

##################################################

print("\nDone")
