LIB = utills.so
LIB_FOLDER = ../Utills/
FILES = basic_defs.c label.c file_handler.c macro.c pre_processor.c parser.c  function.c encoder.c assembler.c main.c
OBJECTS = basic_defs.o label.o file_handler.o macro.o pre_processor.o parser.o function.o encoder.o assembler.o main.o
GCC_FLAGS = -ansi -Wall -pedantic-errors -g
PRODUCT = assembler

$(PRODUCT): objects $(OBJECTS)
	gcc $(GCC_FLAGS) $(OBJECTS) -L$(LIB_FOLDER) $(LIB_FOLDER)$(LIB) -o $(PRODUCT)
	for i in $(OBJECTS); do \
		mv $$i ./objects/$$i; \
	done

objects:
	mkdir objects

$(OBJECTS): $(LIB) $(FILES)
	gcc -I $(LIB_FOLDER) -c $(GCC_FLAGS) $(FILES)

$(LIB):
	$(MAKE) dyn_lib -C $(LIB_FOLDER) 

clean:
	$(MAKE) clean -C $(LIB_FOLDER)
	rm -f $(PRODUCT)
	rm -r -f objects
	rm -f *.o
	rm -f *.am
	rm -f *.obj
	rm -f *.ent
	rm -f *.ext
