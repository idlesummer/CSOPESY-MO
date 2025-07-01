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
