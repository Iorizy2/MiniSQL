# APILIB.o bptree.o Buffer.o Error.o global.o interpreter.o Record.o 

#MiniSQL.o : MiniSQL.cpp ./INTERPRETER/interpreter.h
#	g++ -c MiniSQL.cpp
	
APILIB.o : ./APILIB/APILIB.cpp ./GLOBAL/global.h ./BPLUSTREE/bptree.h ./RECORD/Record.h
	g++ -c ./APILIB/APILIB.cpp
	
.PHONY : clean	
clean :
	rm *.o