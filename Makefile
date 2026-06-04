CXX      ?= g++
CXXFLAGS ?= -std=c++23 -Wall -Wextra -O2 -I.

TARGET := dla_mountain
SRCS   := main.cpp Heightmap.cpp DLA.cpp
OBJS   := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f iter*.png