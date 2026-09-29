# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheaders

# Libraries to link against
LIBS     = -lGLEW -lglfw -lGL

# Source and object files
SRCS     = src/main.cpp src/Shader.cpp src/Utils.cpp src/Quad.cpp src/Camera.cpp src/Ghost.cpp src/Path.cpp
OBJS     = $(SRCS:.cpp=.o)

# Target executable name
TARGET   = main

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

# Compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJS)
