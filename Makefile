# Macros
CC = gcc
# CC = gccm

# Dependency rules for non-file targets
all: ishlex ishsyn ish
clean: 
	rm  -f ishlex ishsyn ish *.o

# Dependency rules for file targets
ishlex: dynarray.o lexAnalyze.o token.o program.o ishlex.o
	$(CC) dynarray.o lexAnalyze.o token.o program.o ishlex.o -o \
	ishlex

ishsyn: dynarray.o lexAnalyze.o synAnalyze.o command.o token.o \
	program.o ishsyn.o
	$(CC) dynarray.o lexAnalyze.o synAnalyze.o command.o token.o \
		program.o ishsyn.o -o ishsyn
ish: dynarray.o lexAnalyze.o synAnalyze.o command.o token.o program.o \
	ish.o
	$(CC) dynarray.o lexAnalyze.o synAnalyze.o command.o token.o \
		program.o ish.o -o ish
ishlex.o: ishlex.c dynarray.h lexAnalyze.h token.h program.h
ishsyn.o: ishsyn.c dynarray.h lexAnalyze.h synAnalyze.h command.h \
	token.h program.h
ish.o: ish.c dynarray.h lexAnalyze.h synAnalyze.h command.h token.h \
	program.h
dynarray.o: dynarray.c dynarray.h
lexAnalyze.o: lexAnalyze.c lexAnalyze.h dynarray.h token.h program.h
synAnalyze.o: synAnalyze.c synAnalyze.h dynarray.h token.h program.h \
	command.h
token.o: token.c dynarray.h token.h program.h
program.o: program.c program.h
command.o: command.c dynarray.h command.h token.h program.h
