import random
import sys

#sys.path.append('./bin')
#from yoyo.genesis.utils import *

sys.path.append('./utils_bindings')
from libutils.genesis.utils import *

##################################################
################# ARITHMETICS ####################

bv0 = Bitvector( "010101010101" );
bv1 = Bitvector( "111000111000" );
bv2 = Bitvector( "111100001111" );

bvz = Bitvector( 12, False );
bvo = Bitvector( 12, True );

# ARITHMETICS-1
if bv0 != (bv0 & bv0):
	print("Error at ARITHMETICS-1")
# ARITHMETICS-2
if bv1 != (bv1 & bv1):
	print("Error at ARITHMETICS-2")
# ARITHMETICS-3
if bv2 != (bv2 & bv2):
	print("Error at ARITHMETICS-3")

# ARITHMETICS-4
if bv0 != (bv0 | bv0):
	print("Error at ARITHMETICS-4")
# ARITHMETICS-5
if bv1 != (bv1 | bv1):
	print("Error at ARITHMETICS-5")
# ARITHMETICS-6
if bv2 != (bv2 | bv2):
	print("Error at ARITHMETICS-6")

# ARITHMETICS-7
if bvz != (bv0 ^ bv0):
	print("Error at ARITHMETICS-7")
# ARITHMETICS-8
if bvz != (bv1 ^ bv1):
	print("Error at ARITHMETICS-8")
# ARITHMETICS-9
if bvz != (bv2 ^ bv2):
	print("Error at ARITHMETICS-9")

# ARITHMETICS-10
if Bitvector( "101010101010" ) != ~bv0:
	print("Error at ARITHMETICS-10")
# ARITHMETICS-11
if Bitvector( "000111000111" ) != ~bv1:
	print("Error at ARITHMETICS-11")
# ARITHMETICS-12
if Bitvector( "000011110000" ) != ~bv2:
	print("Error at ARITHMETICS-12")

# ARITHMETICS-13
if 6 != bv0.count():
	print("Error at ARITHMETICS-13")
# ARITHMETICS-14
if 6 != bv1.count():
	print("Error at ARITHMETICS-14")
# ARITHMETICS-15
if 8 != bv2.count():
	print("Error at ARITHMETICS-15")

# ARITHMETICS-16
if 6 != (~bv0).count():
	print("Error at ARITHMETICS-16")
# ARITHMETICS-17
if 6 != (~bv1).count():
	print("Error at ARITHMETICS-17")
# ARITHMETICS-18
if 4 != (~bv2).count():
	print("Error at ARITHMETICS-18")

# ARITHMETICS-19
if bv0 == bv1:
	print("Error at ARITHMETICS-19")
# ARITHMETICS-20
if bv0 == bv2:
	print("Error at ARITHMETICS-20")
# ARITHMETICS-21
if bv1 == bv2:
	print("Error at ARITHMETICS-21")
# ARITHMETICS-22
if not (bv0 != bv1):
	print("Error at ARITHMETICS-22")
# ARITHMETICS-23
if not (bv0 != bv2):
	print("Error at ARITHMETICS-23")
# ARITHMETICS-24
if not (bv1 != bv2):
	print("Error at ARITHMETICS-24")

##################################################
################## COPY_RANGE ####################

bv_20 = Bitvector( 20 )
bv_10 = Bitvector( 10, bv_20 )

# COPY_RANGE-1
if 10 != bv_10.size():
	print("Error at COPY_RANGE-1")

bv_cp = Bitvector( 20, bv_20 )

# COPY_RANGE-2
if 20 != bv_cp.size():
	print("Error at COPY_RANGE-2")

##################################################
#################### STREAMS #####################

size = 50
bv = Bitvector( size )
for i in range(0, size):
	p = random.randint(0,size-1)
	bv.flip( p )

# 3.1

# 3.2

##################################################
################### BOOL_VEC #####################

# BOOL_VEC-1
v = make_bool_vector_from_indices( [1, 3, 5] )
e = [False, True, False, True, False, True]
if v != e:
	print("Error at BOOL_VEC-1")

# BOOL_VEC-2
v = make_bool_vector_from_indices( [1, 3, 5], 8 )
e = [False, True, False, True, False, True, False, False]
if v != e:
	print("Error at BOOL_VEC-2")

# BOOL_VEC-3
exception_check = False
try:
	make_bool_vector_from_indices( [1, 3, 5], 5 )
except:
	exception_check = True
if not exception_check:
	print("Error at BOOL_VEC-3")

##################################################
################### OPERATORS ####################

bv_s = Bitvector( "0011" )
bv_l = Bitvector( "010101" )

# OPERATORS-1
if Bitvector("0011") != bitwise_and( bv_s, bv_s, False ):
	print("Error at OPERATORS-1")
# OPERATORS-2
if Bitvector("0001") != bitwise_and( bv_l, bv_s, False ):
	print("Error at OPERATORS-2")
# OPERATORS-3
if Bitvector("0001") != bitwise_and( bv_s, bv_l, False ):
	print("Error at OPERATORS-3")
# OPERATORS-4
if Bitvector("010101") != bitwise_and( bv_l, bv_l, False ):
	print("Error at OPERATORS-4")

# OPERATORS-5
if Bitvector("0011") != bitwise_and( bv_s, bv_s, True ):
	print("Error at OPERATORS-5")
# OPERATORS-6
if Bitvector("000100") != bitwise_and( bv_l, bv_s, True ):
	print("Error at OPERATORS-6")
# OPERATORS-7
if Bitvector("000100") != bitwise_and( bv_s, bv_l, True ):
	print("Error at OPERATORS-7")
# OPERATORS-8
if Bitvector("010101") != bitwise_and( bv_l, bv_l, True ):
	print("Error at OPERATORS-8")

# OPERATORS-9
if Bitvector("0011") != bitwise_or( bv_s, bv_s, False ):
	print("Error at OPERATORS-9")
# OPERATORS-10
if Bitvector("0111") != bitwise_or( bv_l, bv_s, False ):
	print("Error at OPERATORS-10")
# OPERATORS-11
if Bitvector("0111") != bitwise_or( bv_s, bv_l, False ):
	print("Error at OPERATORS-11")
# OPERATORS-12
if Bitvector("010101") != bitwise_or( bv_l, bv_l, False ):
	print("Error at OPERATORS-12")

# OPERATORS-13
if Bitvector("0011") != bitwise_or( bv_s, bv_s, True ):
	print("Error at OPERATORS-13")
# OPERATORS-14
if Bitvector("011101") != bitwise_or( bv_l, bv_s, True ):
	print("Error at OPERATORS-14")
# OPERATORS-15
if Bitvector("011101") != bitwise_or( bv_s, bv_l, True ):
	print("Error at OPERATORS-15")
# OPERATORS-16
if Bitvector("010101") != bitwise_or( bv_l, bv_l, True ):
	print("Error at OPERATORS-16")

# OPERATORS-17
if Bitvector("0000") != bitwise_xor( bv_s, bv_s, False ):
	print("Error at OPERATORS-17")
# OPERATORS-18
if Bitvector("0110") != bitwise_xor( bv_l, bv_s, False ):
	print("Error at OPERATORS-18")
# OPERATORS-19
if Bitvector("0110") != bitwise_xor( bv_s, bv_l, False ):
	print("Error at OPERATORS-19")
# OPERATORS-20
if Bitvector("000000") != bitwise_xor( bv_l, bv_l, False ):
	print("Error at OPERATORS-20")

# OPERATORS-21
if Bitvector("0000") != bitwise_xor( bv_s, bv_s, True ):
	print("Error at OPERATORS-21")
# OPERATORS-22
if Bitvector("011001") != bitwise_xor( bv_l, bv_s, True ):
	print("Error at OPERATORS-22")
# OPERATORS-23
if Bitvector("011001") != bitwise_xor( bv_s, bv_l, True ):
	print("Error at OPERATORS-23")
# OPERATORS-24
if Bitvector("000000") != bitwise_xor( bv_l, bv_l, True ):
	print("Error at OPERATORS-24")

##################################################

print("\nDone")
