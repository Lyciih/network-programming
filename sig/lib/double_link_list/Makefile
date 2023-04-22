

SRCS_C = \
	DLL_add_first.c \
	DLL_add_tail.c \
	DLL_addto_next.c \
	DLL_addto_prev.c \
	DLL_concate.c \
	DLL_delete.c \
	DLL_free_head.c \
	DLL_get_tail.c \
	DLL_init.c \
	DLL_isEmpty.c \
	DLL_next_node.c \
	DLL_num_nodes.c \
	

TEST_C = \
	test.c \

OBJS = $(SRCS_C:.c=.o)
TEST = $(TEST_C:.c=.o)

INCLUDE = -I ./include


all: test

test: libdouble_linked_list.a ${TEST} 
	gcc -o test  $(INCLUDE) test.o -L . -l double_linked_list  


%.o : %.c
	gcc -c -o $@ $< $(INCLUDE)

%.a : ${OBJS}
	ar -rcs libdouble_linked_list.a  $^ 

%.o : ./src/%.c 
	gcc -c -o $@ $< $(INCLUDE)

.PHONY: clean run
clean:
	rm -f test *.o *.a

run:
	./test
