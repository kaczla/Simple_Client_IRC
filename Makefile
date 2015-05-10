CXX=g++
CXXFLAGS=-std=c++11 -Ofast
LFLAGS=-pthread
DIR=src
SOURCE=client.cpp
OUT_SOURCE=client.o
APP_NAME=client

all: source main
	
main:
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(DIR)/main.cpp $(OUT_SOURCE) -o $(APP_NAME)
source:
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(DIR)/$(SOURCE) -c

clean:
	rm *.o $(APP_NAME)
