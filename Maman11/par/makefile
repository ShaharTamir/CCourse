LIB = utills.so
LIB_FOLDER = ../utills/
FILES = par.c
OBJECTS = par.o
GCC_FLAGS = -ansi -Wall -pedantic-errors
PRODUCT = par

$(PRODUCT): objects $(OBJECTS)
	gcc -g $(GCC_FLAGS) $(OBJECTS) -L$(LIB_FOLDER) $(LIB_FOLDER)$(LIB) -o $(PRODUCT)
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
