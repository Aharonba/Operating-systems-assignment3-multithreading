CXX = g++
CXXFLAGS = -Wall -g -std=c++14 

# Executable name
TARGET = demo

# Source files
SRCS = main.cpp Graph_deque.cpp 

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

# Run profiling
profile: $(TARGET)
	./$(TARGET)

# Phony targets to avoid conflicts with files named 'clean' or 'valgrind'
.PHONY: clean valgrind profile
