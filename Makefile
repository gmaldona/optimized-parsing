# Graduate student @ Thomas J. Watson College of Engineering and Applied
# Sciences, Binghamton University.
#
# author: Gregory Maldonado
# email : gmaldonado@cs.binghamton.edu
# date  : 2024-02-14

#=========== GM ======================================================== 80 ====

CC     = g++
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O
TARGET = 1brc

.pre:
	mkdir -p build

# 		Using Clion to run a debugger. Only target one at a time.
# 			   v
all: optimized # standard

default: optimized

.PHONY: optimized
optimized: optimized.o
	$(CC) $(CFLAGS) -o build/optimized optimized.cpp
	$(RM) optimized.o

optimized.o: .pre optimized.cpp optimized.hpp
	$(CC) $(CFLAGS) -c optimized.cpp

.PHONY: standard
standard: standard.o
	$(CC) $(CFLAGS) -o build/standard standard.cpp
	$(RM) standard.o

standard.o: .pre standard.cpp standard.hpp
	$(CC) $(CFLAGS) -c standard.cpp

.PHONY: clean
clean:
	$(RM) -r build

#=========== GM ======================================================== 80 ====