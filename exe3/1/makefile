# Compiler
CXX = g++
CXXFLAGS = -Wall -g -std=c++11  # Flags: -Wall for warnings, -g for debugging symbols, -std=c++11 for C++11 standard

# Executable name
TARGET = demo

# Source files
SRCS = main.cpp Graph.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile the object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run Valgrind for memory leak check
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Phony targets to avoid conflicts with files named 'clean' or 'valgrind'
.PHONY: clean valgrind
