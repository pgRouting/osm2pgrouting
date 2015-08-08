# osm2pgrouting

## Requirements

Before you can use this tool for importing Openstreetmap data you need to install:

1. postgresql
2. postgis
3. pgrouting
4. boost
5. expat
6. cmake

and to prepare a database.

## Documentation

See in the documentation of the pgrouting website for more informations: http://pgrouting.org

## Installation

For compiling this tool, you will need boost, libpq, expat and cmake:
Then just type the following in the root directory:

```
cmake -H. -Bbuild
cd build/
make
make install
```

## How to use

Start the program like this:

```
osm2pgrouting -file your-OSM-XML-File.osm -conf mapconfig.xml -dbname routing -user postgres -clean
```

A complete list of arguments are:

```
Allowed options:

Help:
  --help                Produce help message for this version.

General:
  -f [ --file ] arg                     Name of your osm file (Required).
  -c [ --conf ] arg (=/usr/share/osm2pgrouting/mapconfig.xml)
                                        Name of your configuration xml file.
  --prefix arg (=planet_)               Prefix added at the beginning of table 
                                        names.
  --suffix arg                          Suffix added at the end of table names.
  -s [ --skipnodes ] arg (=1)           When ture: dont import the node table.
  --clean arg (=0)                      When true: Drop previously created 
                                        tables.

Database options:
  -d [ --dbname ] arg            Name of your database (Required).
  -u [ --user ] arg (=mapas)     Name of the user, which have write access to 
                                 the database.
  -h [ --host ] arg (=localhost) Host of your postgresql database.
  -p [ --db_port ] arg (=5432)   db_port of your database.
  --passwd arg                   Password for database access.

```
