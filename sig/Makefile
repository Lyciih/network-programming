CFLAG = -Wall -g -I include -MD

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, object/%.o, ${SRCS})
DEPS = $(OBJS:.o=.d)

all: shell bin/number copy 

-include $(DEPS)

shell: $(OBJS)
	gcc $(CFLAG) -o $@ $^ lib/libdouble_linked_list.a

object/%.o: src/%.c
	gcc $(CFLAG) -c $< -o $@

bin/number: bin/number.cpp
	g++ bin/number.cpp -o bin/number



server: ./src/server.c
	gcc $(CFLAG) ./src/server.c -o server lib/libdouble_linked_list.a


.PHONY: clean run netstat copy test

copy:
	cp /bin/ls ./bin
	cp /bin/cat ./bin
	cp /bin/more ./bin

clean:
	rm -f server shell object/*.o object/*.d bin/number bin/ls bin/more bin/cat

run:
	./shell

netstat:
	netstat -a|grep 3000

test:
	clear
	telnet 59.127.115.95 1223
