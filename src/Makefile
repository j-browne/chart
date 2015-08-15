CC      = clang++
CFLAGS  = -Wall -Wextra -march=native -std=c++11
CFLAGS += -O3
CFLAGS += -pedantic -g

COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

TARGET = chart
OBJS = chart.o cmdline.o # List of object dependencies

$(TARGET): $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

cmdline.c: chart_opts.ggo
	gengetopt < chart_opts.ggo

cmdline.o: cmdline.c
	clang -c cmdline.c

%.o:%.cpp
	$(COMPILE) -c $<

clean: 
	rm $(OBJS)
