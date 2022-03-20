FILES = node.c linked_list.c basic_defs.c label.c file_handler.c macro.c pre_processor.c parser.c  function.c encoder.c assembler.c main.c
OBJECTS = node.o linked_list.o basic_defs.o label.o file_handler.o macro.o pre_processor.o parser.o function.o encoder.o assembler.o main.o
GCC_FLAGS = -ansi -Wall -pedantic-errors -g
PRODUCT = assembler
TESTS_FOLDER = full_tests/
OUTPUTS = *.obj *.ent *.ext *.am *.out
TESTS = ps data_in_mid

$(PRODUCT): objects $(OBJECTS)
	gcc $(GCC_FLAGS) $(OBJECTS) -o $(PRODUCT)
	for i in $(OBJECTS); do \
		mv $$i ./objects/$$i; \
	done

objects:
	mkdir objects

$(OBJECTS): $(FILES)
	gcc -c $(GCC_FLAGS) $(FILES)

run-all:
	for i in $(TESTS); do \
		./$(PRODUCT) $(TESTS_FOLDER)$$i; \
	done

clean-outputs: 
	for i in $(OUTPUTS); do \
		rm -f $(TESTS_FOLDER)$$i; \
	done

clean: clean-outputs
	rm -f $(PRODUCT)
	rm -r -f objects
	rm -f *.o
	rm -f *.am