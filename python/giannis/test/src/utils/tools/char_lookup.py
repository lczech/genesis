import sys

sys.path.append('./bin')

from yoyo.genesis.utils import *

##################################################
#################### SIMPLE ######################

cl = CharLookup_bool_t()

cl.set_selection( "abc", True );
# SIMPLE-1
if not cl.get('a'):
	print("Error at SIMPLE-1")
# SIMPLE-2
if cl.get('A'):
	print("Error at SIMPLE-2")

cl.set_range( 'G', 'L', True );
# SIMPLE-3
if cl.get('F'):
	print("Error at SIMPLE-3")
# SIMPLE-4
if not cl.get('I'):
	print("Error at SIMPLE-4")

cl.set_char( 'I', False );
# SIMPLE-5
if cl.get('I'):
	print("Error at SIMPLE-5")

##################################################

print("\nDone")
