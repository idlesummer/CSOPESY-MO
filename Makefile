# === Quick Compile & Run (for devs) ===
# Use this one-liner if you don't have make installed:
#   g++ -std=c++20 -I src/ -o main src/main.cpp && main

# === Compiler and Flags ===
CXX := g++
CXXFLAGS := -std=c++20 -I src/

# === Targets ===
TARGET := main
SRC := src/main.cpp

# === Default rule: build and run ===
all: $(TARGET)
	./$(TARGET)

# === Compile target ===
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# === Clean rule ===
clean:
	rm -f $(TARGET)

# === Bundle rule ===
bundle:
	powershell -ExecutionPolicy Bypass -File bundle.ps1

.PHONY: all clean
