CC      = clang++
CFLAGS  = -Wall -Wextra -march=native -std=c++11
CFLAGS += -O3
CFLAGS += -pedantic -g

COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

TARGET = chart
OBJS = chart.o  # List of object dependencies

$(TARGET): $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

%.o:%.cpp
	$(COMPILE) -c $<

clean: 
	rm $(OBJS)
