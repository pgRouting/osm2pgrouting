CC = g++

SRC = src/Export2DB.cpp src/math_functions.cpp src/Node.cpp src/Tag.cpp src/OSMDocumentParserCallback.cpp src/Way.cpp src/OSMDocument.cpp src/Type.cpp src/Class.cpp src/Configuration.cpp src/ConfigurationParserCallback.cpp src/Relation.cpp

DEPS = src/XMLParser.cpp

INC = -I./ -Isrc -I/usr/include/pgsql -I/usr/include/postgresql -I/usr/local/pgsql/include

LIB_DIRS = -L/usr/local/pgsql/lib -L/usr/local/lib/pgsql

MAIN = src/osm2pgrouting.cpp

all:
	$(CC) -c $(SRC) $(INC) -ggdb3
	$(CC) -c $(DEPS) $(INC) -ggdb3
	$(CC) -o osm2pgrouting $(MAIN) *.o $(INC) -lexpat -ggdb3 $(LIB_DIRS) -lpq
	rm *.o
clean:
	rm -f *.o osm2pgrouting

