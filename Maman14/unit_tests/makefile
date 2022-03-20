PARSER_FILES = tester.c ../parser.c ../basic_defs.c parser_tester.c
LIST_FILES = tester.c ../node.c ../linked_list.c linked_list_tester.c
FILE_FILES = ../file_handler.c file_handler_tester.c

parser:
	gcc -o parser_tester.out $(PARSER_FILES)

list:
	gcc -o list_tester.out $(LIST_FILES)

file:
	gcc -o file_handler_tester.out $(FILE_FILES)

clean:
	rm -f *.o
	rm -f *.out