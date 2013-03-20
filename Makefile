CC = g++

CFLAGS = -ggdb3 -D_FILE_OFFSET_BITS=64

OBJ = bin/Export2DB.o bin/math_functions.o bin/Node.o bin/OSMDocumentParserCallback.o bin/Way.o bin/OSMDocument.o bin/Type.o bin/Class.o bin/Configuration.o bin/ConfigurationParserCallback.o bin/Relation.o bin/XMLParser.o

INC_DIRS = -I./ -Isrc -I/usr/include/pgsql -I/usr/include/postgresql -I/usr/local/pgsql/include

LIB_DIRS = -L/usr/local/pgsql/lib -L/usr/local/lib/pgsql

LIBS = -lexpat -lpq

MAIN = src/osm2pgrouting.cpp

all: osm2pgrouting

osm2pgrouting : bin $(OBJ) $(MAIN)
	$(CC) $(CFLAGS) $(MAIN) $(OBJ) $(INC_DIRS) $(LIB_DIRS) $(LIBS) -o bin/$@
	

bin:
	mkdir -p bin
  
bin/%.o : src/%.cpp
	$(CC) -o $@ $(INC_DIRS) $(CFLAGS) -c $<

clean:
	rm -rf bin
	rm -f osm2pgrouting
