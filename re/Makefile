CFLAG = -Wall -g -I include -MD

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, object/%.o, ${SRCS})
DEPS = $(OBJS:.o=.d)



all: shell bin/number COPY

-include $(DEPS)  #這行如果放在 all 之前,修改 .h 檔後 , make只會更新 .o 檔，最終的執行檔不會重新產生




shell: $(OBJS)
	gcc -Wall -g -o $@ $^ lib/libdouble_linked_list.a 

object/%.o: src/%.c
	gcc $(CFLAG) -c $<  -o $@

bin/number: bin/number.cpp
	g++ bin/number.cpp -o bin/number

COPY:
	cp /bin/ls ./bin
	cp /bin/cat ./bin
	cp /bin/more ./bin

.PHONY: clean run

clean:
	rm -f shell object/*.o object/*.d bin/number bin/ls bin/more bin/cat
run:
	./shell
