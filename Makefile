bin = cloNES
src = $(wildcard source/*.cpp source/mapper/*.cpp source/common/*.cpp emulator.cpp main.cpp)
obj = $(src:.cpp=.o)

CXX = g++
CXXFLAGS = -g -Wall -Wextra -O2 -std=c++14 -pedantic $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2)

$(bin): $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	-rm $(bin) $(obj)
