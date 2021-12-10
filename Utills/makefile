STACK_OBJ = stack_tester.o
STATE_OBJ = state_machine_tester.o
FILES = stack.c state_machine.c test_input.c tester.c
OBJECTS = stack.o state_machine.o test_input.o tester.o
FLAGS = -ansi -Wall -pedantic-errors

dyn_lib: $(OBJECTS)
	gcc -g -shared $(FLAGS) $(OBJECTS) -o utills.so
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

clean:
	rm -f *.so
	rm -f *.a
	rm -f *.o
	rm -f *.out
	rm -r -f objects	