CFLAG = -Wall -g -I include -MD

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, ${SRCS})
DEPS = $(OBJS:.o=.d)



all: a.out

-include $(DEPS)  #這行如果放在 all 之前,修改 .h 檔後 , make只會更新 .o 檔，最終的執行檔不會重新產生

a.out: $(OBJS)
	gcc -Wall -g -o $@ $^ 

obj/%.o: src/%.c
	gcc $(CFLAG) -c $<  -o $@



.PHONY: clean run

clean:
	rm -rf a.out obj/*.o obj/*.d
run:
	./a.out
