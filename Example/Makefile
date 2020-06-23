NAME := ParseTest

CXX := clang++
CXXFLAGS := -std=c++2a -O2 -Wall

SRCS := $(shell find . -name "*.cpp")
OBJS := $(subst .cpp,.o,$(SRCS))

all: $(NAME) clean

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(LIBS)

.PHONY: run
run:
	./$(NAME)

.PHONY: clean
clean:
	@echo $(OBJS)
	rm -f $(OBJS)