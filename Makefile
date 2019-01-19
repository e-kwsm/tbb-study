CXX      := clang++
CXXFLAGS := -std=c++17
CPPFLAGS :=
LDFLAGS  :=
LDLIBS   := -ltbb
SRCS     := $(wildcard *.cpp)
TARGET   := $(SRCS:.cpp=)

%: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
