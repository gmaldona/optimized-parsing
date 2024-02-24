# Graduate student @ Thomas J. Watson College of Engineering and Applied
# Sciences, Binghamton University.
#
# author: Gregory Maldonado
# email : gmaldonado@cs.binghamton.edu
# date  : 2024-02-14

CC     = g++
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O
TARGET = 1brc

.pre:
	mkdir -p build

.PHONY: optimized
optimized: .optimized
	$(RM) optimized.o

.PHONY: standard
standard: .standard
	$(RM) standard.o

.optimized: optimized.o
	$(CC) $(CFLAGS) -o build/optimized optimized.cpp

.optimized.o: .pre optimized.cpp optimized.hpp
	$(CC) $(CFLAGS) -c optimized.cpp

.standard: standard.o
	$(CC) $(CFLAGS) -o build/standard standard.cpp

.standard.o: .pre standard.cpp standard.hpp
	$(CC) $(CFLAGS) -c standard.cpp

.PHONY: clean
clean:
	$(RM) -r build