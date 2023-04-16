TARGET=build/snake-fab
CFLAGS=-W -Wall -pedantic -std=c99 $(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs) -lSDL2_image -lm

SRCS=$(wildcard source/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

-include $(DEPS)

%.o: %.c %.d
	$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	$(CC) $(CFLAGS) -c $< -MM -MT $(@:.d=.o) > $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
