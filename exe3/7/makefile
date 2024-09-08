# Compiler
CXX = g++
CXXFLAGS = -Wall -g -std=c++14  # Use C++14 standard

# Executable names
SERVER_TARGET = server
CLIENT_TARGET = client

# Source files
SERVER_SRCS = Server.cpp Graph_deque.cpp
CLIENT_SRCS = Client.cpp Graph_deque.cpp

# Object files
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

# Default target
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Build the server executable
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS)

# Build the client executable
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

# Compile the object files for the server
server_objs: $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) -c $(SERVER_SRCS)

# Compile the object files for the client
client_objs: $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -c $(CLIENT_SRCS)

# Clean up object files and the executables
clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_TARGET) $(CLIENT_TARGET)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: all clean
