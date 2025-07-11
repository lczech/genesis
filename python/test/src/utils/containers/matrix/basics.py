import sys

sys.path.append('./bin')

from yoyo.genesis.utils import *

##################################################
#################### COPY ########################

m1 = Matrix_int_t(2, 2, [1, 2, 3, 4])
m2 = m1
m3 = Matrix_int_t(m1)

# COPY-1
if m1 != m2:
	print("Error at COPY-1")
# COPY-2
if m1 != m3:
	print("Error at COPY-2")

# COPY-3
# COPY-4

# COPY-5
# COPY-6

#m(0,1) = 9

# COPY-7
# COPY-8

# COPY-9
# COPY-10

# COPY-11
# COPY-12

# COPY-13
# COPY-14
# COPY-15

##################################################
############### INITIALIZER_LIST #################

# INITIALIZER_LIST-1
exception_check = False
try:
	Matrix_int_t(2, 2, [1, 2, 3])
except ValueError:
	exception_check = True
if not exception_check:
	print("Error at INITIALIZER_LIST-1")

# INITIALIZER_LIST-2
exception_check = False
try:
        Matrix_int_t(2, 2, [1, 2, 3, 4])
except ValueError:
        exception_check = True
if exception_check:
        print("Error at INITIALIZER_LIST-2")

# INITIALIZER_LIST-3
exception_check = False
try:
        Matrix_int_t(2, 2, [1, 2, 3, 4, 5])
except ValueError:
        exception_check = True
if not exception_check:
        print("Error at INITIALIZER_LIST-3")

##################################################
#################### ACCESS #####################

m = Matrix_int_t(2, 2, [1, 2, 3, 4])

# ACCESS-1
if 2 != m.rows():
	print("Error at ACCESS-1")
# ACCESS-2
if 2 != m.cols():
	print("Error at ACCESS-2")

# ACCESS-3
if 2 != m(0,1):
	print("Error at ACCESS-3")

# ACCESS-4
exception_check = False
try:
	m.at(0, 2)
except IndexError:
	exception_check = True
if not exception_check:
	print("Error at ACCESS-4")

##################################################
################ SWAP_ROWS_COLS ##################

m = Matrix_int_t(4, 4, [1,  2,  3,  4, 5,  6,  7,  8, 9, 10, 11, 12, 13, 14, 15, 16])
mre = Matrix_int_t(4, 4, [1,  2,  3,  4, 13, 14, 15, 16, 9, 10, 11, 12, 5,  6,  7,  8 ])

swap_rows_Matrix_int_t( m, 1, 3)
# SWAP_ROWS_COLS-1
if mre != m:
	print("Error at SWAP_ROWS_COLS-1")

mce = Matrix_int_t(4, 4, [2, 1, 3, 4, 14, 13, 15, 16, 10,  9, 11, 12, 6,  5,  7,  8])

swap_cols_Matrix_int_t( m, 1, 0 );
# SWAP_ROWS_COLS-2
if mce != m:
	print("Error at SWAP_ROWS_COLS-2")

##################################################
#################### OSTREAM #####################

m = Matrix_int_t(2, 2, [1, 2, 3, 4])

#s = ""
#s.__lshift__(m)
#s << m

##################################################
############## TRIANGULAR_INDICES ################

n = 1000
k = 0

for i in range( n):
	for j in range( i+1, n):
		p = triangular_indices( k, n)
		k2 = triangular_index( i, j, n)

		if i != p[0]:
			print("Error at TRIANGULAR_INDICES-1")
		if j != p[1]:
			print("Error at TRIANGULAR_INDICES-2")
		if k != k2:
			print("Error at TRIANGULAR_INDICES-3")

		k += 1
##################################################

print("\nDone")
