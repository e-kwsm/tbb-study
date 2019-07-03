CXX      := clang++
CXXFLAGS := -std=c++17
LDLIBS   := -ltbb
TARGET   := $(patsubst %.cpp,%,$(wildcard *.cpp))

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
