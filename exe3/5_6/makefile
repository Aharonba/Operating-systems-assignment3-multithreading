# Compiler
CXX = g++
CXXFLAGS = -Wall -g -std=c++14  # Use C++14 standard

# Executable names
SERVER_TARGET = server
CLIENT_TARGET = client
REACTOR_LIB = libreactor.a  # Static library for Reactor

# Source files
SERVER_SRCS = Server.cpp Graph_deque.cpp
CLIENT_SRCS = Client.cpp Graph_deque.cpp
REACTOR_SRCS = Reactor.cpp  # Reactor source file from Exercise 5

# Object files
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
REACTOR_OBJS = $(REACTOR_SRCS:.cpp=.o)

# Default target
all: $(REACTOR_LIB) $(SERVER_TARGET) $(CLIENT_TARGET)

# Build the Reactor library (static library)
$(REACTOR_LIB): $(REACTOR_OBJS)
	ar rcs $(REACTOR_LIB) $(REACTOR_OBJS)

# Build the server executable (link with Reactor library)
$(SERVER_TARGET): $(SERVER_OBJS) $(REACTOR_LIB)
	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS) -L. -lreactor

# Build the client executable (link with Reactor library if needed)
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

# Compile the object files for the server
server_objs: $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) -c $(SERVER_SRCS)

# Compile the object files for the client
client_objs: $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -c $(CLIENT_SRCS)

# Clean up object files, executables, and the library
clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(REACTOR_OBJS) $(SERVER_TARGET) $(CLIENT_TARGET) $(REACTOR_LIB)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: all clean
