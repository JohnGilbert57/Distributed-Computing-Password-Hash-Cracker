# Generic OpenMPI Makefile
# Ostermann - Mar 15, 2022

CC=mpicc -Wall -Werror -O2 -Wno-unused-result


CSOURCES=${wildcard *.c}
TARGET=hash_cracker

all: ${TARGET}	

${TARGET}: ${CSOURCES}

test: ${TARGET}	
	echo "foobar 	 9pHdGraWcEy3y.NvdzCOSfu0XalZhBWUgJ/iKxpdipC  01  8"  	 	| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: 204: '11001100'"
	echo "notfound 	 9pHdGraWcEy3y.NvdzCOSfu0XalZhBWUgJ/iKxpdipC  23  8"  	 	| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: not found"
	echo "foobar 	 P0jyUFlvXHFKF8.IDZKc.9dXfKDvfmFv7sD6VFxFnE1  01 10"  		| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: 819: '1100110011'"
	echo "vowelsRfun lFtqQCPzGxs/jOa1WSAVq1A40onc4iV1WcBdcVgxKV0  aeiou  5"		| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: 1730: 'iouea'"
	echo "hexydigits BKD9vk1zOVvrNd8xRZR7B5cEsqrd61b9IUmPGSu9.c2  0123456789ABCDEF  3"		| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: 3150: 'C4E'"
	echo "andpepper	 Xl9dddHVkCrdG3vTJkuEBK9ecqr/aZT5wJFKqz/PRJC  clangCLANK  4"  		| mpiexec -n 4 ./hash_cracker
	@echo "Correct answer: 8251: 'NaCl'"
	echo "rnderr     eNQG/ksOHKtFACs9sjiPMHvwiaSVFMiKOgtGUNVX9r3  01  8"        	| mpiexec -n 3 ./hash_cracker
	@echo "Correct answer: 255: '11111111'"

test_ssh:
	ssh odd01 exit
	ssh odd05 exit
	ssh odd07 exit
	ssh odd09 exit
	ssh odd11 exit
	ssh odd13 exit
	ssh odd15 exit
	ssh odd17 exit
	ssh odd19 exit
	ssh odd21 exit
	ssh odd23 exit
	ssh odd25 exit
	ssh odd27 exit
	ssh odd29 exit
	ssh odd31 exit
	ssh odd33 exit
	ssh odd35 exit
	ssh odd37 exit
	ssh odd39 exit
	ssh odd41 exit
	ssh pu3 exit

clean:
	rm -f  *.o ${TARGET}
