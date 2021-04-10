editor: editor.o init.o gui.o
		gcc -Wall editor.o init.o gui.o -o editor -lncurses
editor.o: editor.c editor.h syntaxHL.h
		gcc -Wall -c editor.c 
init.o: init.c init.h editor.h
		gcc -Wall -c init.c 
gui.o: gui.c gui.h editor.h
		gcc -Wall -c gui.c 