
CC = g++

SRC = ../src/Export2DB.cpp	../src/math_functions.cpp ../src/Node.cpp ../src/OSMDocumentParserCallback.cpp ../src/Way.cpp ../src/OSMDocument.cpp 

DEPS = ../src/XMLParser.cpp

INC = -I./ -I../src 

MAIN = ../osm2pgsql.cpp

all:
	$(CC) -c $(SRC) $(INC) -ggdb3 -DRESTRICT
	$(CC) -c $(DEPS) $(INC) -ggdb3
	$(CC) -o main $(MAIN) *.o $(INC) -lexpat -ggdb3 -lpq
clean:
	rm *.o main

