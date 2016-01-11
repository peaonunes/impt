CC=g++
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

all: ipmt

ipmt: $(OBJ)
	@mkdir -p bin
	g++ -O3 $(OBJ) -o bin/ipmt

debug: $(OBJ)
	@mkdir -p bin
	g++ -O3 $(OBJ) -o bin/ipmt


$(OBJ): $(SRC)
	@mkdir -p $(@D)
	g++ -O3 -std=c++11 -g -c $(patsubst obj/%.o, src/%.cpp, $@) -o $@


run: ipmt
	./bin/pmt $(args)

clean:
	rm -rf obj/*
