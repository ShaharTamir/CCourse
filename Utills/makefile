STACK_OBJ = stack_tester.o
STATE_OBJ = state_machine_tester.o
LIST_OBJ = linked_list_tester.o
FILES = stack.c state_machine.c linked_list.c node.c test_input.c tester.c
OBJECTS = stack.o state_machine.o linked_list.o node.o test_input.o tester.o
FLAGS = -g -ansi -Wall -pedantic-errors

dyn_lib: $(OBJECTS)
	gcc -shared $(FLAGS) $(OBJECTS) -o utills.so
	mkdir -p ./objects
	for i in $(OBJECTS); do \
		mv $$i ./objects/$$i; \
	done

lib: $(OBJECTS)
	ar -cvq utills $(OBJECTS)
	mv ./utills ./utills.a 
	mkdir -p ./objects
	for i in $(OBJECTS); do \
		mv $$i ./objects/$$i; \
	done
	
$(OBJECTS): $(FILES)
	gcc -c -fPIC $(FLAGS) $(FILES)

stack: lib $(STACK_OBJ) 
	gcc -o stack.out $(STACK_OBJ) utills.a
	mv $(STACK_OBJ) ./objects/$(STACK_OBJ)

$(STACK_OBJ):
	gcc -c $(FLAGS) stack_tester.c

state_machine: lib $(STATE_OBJ)
	gcc -o state_machine.out $(STATE_OBJ) utills.a
	mv $(STATE_OBJ) ./objects/$(STATE_OBJ)

$(STATE_OBJ):
	gcc -c $(FLAGS) state_machine_tester.c

list: lib $(LIST_OBJ)
	gcc -o linked_list.out $(LIST_OBJ) utills.a
	mv $(LIST_OBJ) ./objects/$(LIST_OBJ)

$(LIST_OBJ):
	gcc -c $(FLAGS) linked_list_tester.c

clean:
	rm -f *.so
	rm -f *.a
	rm -f *.o
	rm -f *.out
	rm -r -f objects	
