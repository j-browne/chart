CC      = clang++
CFLAGS  = -Wall -Wextra -march=native -std=c++11
CFLAGS += -O3
CFLAGS += -pedantic -g

COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

TARGET = chart
OBJS = cmdline.o chart.o # List of object dependencies

$(TARGET): $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

cmdline.c cmdline.h: chart_opts.ggo
	gengetopt < chart_opts.ggo

cmdline.o: cmdline.c cmdline.h
	clang -c cmdline.c

chart.o: chart.cpp cmdline.h
	$(COMPILE) -c chart.cpp

%.o:%.cpp
	$(COMPILE) -c $<

clean: 
	rm $(OBJS)
