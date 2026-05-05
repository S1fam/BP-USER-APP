CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -O0
INCLUDES := -Iexternal/include -Imodel -Iparser -Isimulator -Ivalidator -Iutils -Icli

SRC_DIRS := model parser simulator validator utils cli .
BUILD    := build

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS := $(patsubst %.cpp,$(BUILD)/%.o,$(SRCS))

TARGET := automaton

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD) $(TARGET)

INPUT := "{a,a,b,b,c,c}"
run: all
	./$(TARGET) json-examples/HZA.json $(INPUT)

.PHONY: all clean run