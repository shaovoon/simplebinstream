# Project: test_bin_stream

#CPP      = clang++ 
CPP      = g++ 
CC       = gcc
OBJ      = TestBinStream.o $(RES)
LINKOBJ  = TestBinStream.o $(RES)
LIBS     = 
CXXINCS  = 
BIN      = test_bin_stream
CXXFLAGS = -Wall -g -O1 $(CXXINCS)
CFLAGS   = -Wall -g -O1  
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before test_bin_stream all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "test_bin_stream" $(LIBS)

TestBinStream.o: TestBinStream.cpp
	$(CPP) -c TestBinStream.cpp -o TestBinStream.o $(CXXFLAGS)

