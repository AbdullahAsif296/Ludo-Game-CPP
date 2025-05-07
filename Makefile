# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread
# Add these lines after CXXFLAGS if needed
SFML_INCLUDE = -I/path/to/sfml/include
SFML_LIB_PATH = -L/path/to/sfml/lib
CXXFLAGS += $(SFML_INCLUDE)
# SFML settings
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Project files
SRCS = main.cpp Player.cpp global.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = ludo_game

# Build rules
all:$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(SFML_LIB_PATH) $(SFML_LIBS)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Run rule
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g
debug: clean all

.PHONY: all clean run debug 