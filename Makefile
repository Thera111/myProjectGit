# Makefile for Hot Word Statistics System
# 热词统计与分析系统 Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
DEBUG_FLAGS = -g -DDEBUG

# Target executable
TARGET = hotword

# Source files
SOURCES = main.cpp

# Include directories
INCLUDES = -I. -I./cppjieba

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Run the program with default configuration
run: $(TARGET)
	./$(TARGET)

# Run with specific input and output files
# Usage: make run-with INPUT=input2.txt OUTPUT=output2.txt
run-with: $(TARGET)
	./$(TARGET) $(INPUT) $(OUTPUT)

# Clean build artifacts
clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f output*.txt

# Clean everything including output
clean-all: clean
	rm -f *.txt.bak

# Install (copy to /usr/local/bin) - requires sudo
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the program (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  run       - Build and run with default config"
	@echo "  run-with  - Build and run with INPUT and OUTPUT variables"
	@echo "  clean     - Remove build artifacts and output files"
	@echo "  clean-all - Remove all generated files"
	@echo "  install   - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make debug"
	@echo "  make run"
	@echo "  make run-with INPUT=input2.txt OUTPUT=output2.txt"

# Phony targets
.PHONY: all debug run run-with clean clean-all install uninstall help
