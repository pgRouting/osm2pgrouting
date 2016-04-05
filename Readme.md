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

If you have libraries installed in non-standard locations, you might need to pass in parameters to cmake.  Commonly useful parameters are

Install some prerqeuisites

```
sudo apt-get install expat
sudo apt-get install libexpat1-dev
```

CMAKE options:

    -DBOOST_ROOT:PATH=/path/to/boost  folder that contains include, lib, bin directories for boost
    
    -DEXPATH_INCLUDE_DIR:PATH=/path/to/expat/include  the include folder for where your expat is installed
    
    -DPOSTGRESQL_INCLUDE_DIR:PATH=/path/to/postgresql/include  the include folder for postgresql development headers
    
    
A cmake with custom options might look something like

```
cmake -DBOOST_ROOT:PATH=/local/projects/rel-boost-1.58.0 \
    -DPOSTGRESQL_INCLUDE_DIR:PATH=/local/projects/rel-pg94/include  -Bbuild
```

## How to use

Prepare the database:

```
createdb routing
psql -dbname routing -c 'CREATE EXTENSION postgis'
psql -dbname routing -c 'CREATE EXTENSION pgRouting'
```

Start the program like this:

```
osm2pgrouting --f your-OSM-XML-File.osm --conf mapconfig.xml --dbname routing --username postgres --clean
```

Do incremental adition of data without using --clean

```
osm2pgrouting --f next-OSM-XML-File.osm --conf mapconfig.xml --dbname routing --username postgres
```


A complete list of arguments are:

```
Allowed options:


Allowed options:

Help:
  --help                Produce help message for this version.
  -v [ --version ]      Print version string

General:
  -f [ --file ] arg                     REQUIRED: Name of the osm file.
  -c [ --conf ] arg (=/usr/share/osm2pgrouting/mapconfig.xml)
                                        Name of the configuration xml file.
  --schema arg                          Database schema to put tables.
                                          blank: defaults to default schema 
                                                dictated by PostgreSQL 
                                                search_path.
  --prefix arg                          Prefix added at the beginning of the 
                                        table names.
  --suffix arg                          Suffix added at the end of the table 
                                        names.
  --addnodes                            Import the osm_nodes table.
  --clean                               Drop previously created tables.

Database options:
  -d [ --dbname ] arg               Name of your database (Required).
  -U [ --username ] arg (=postgres) Name of the user, which have write access 
                                    to the database.
  -h [ --host ] arg (=localhost)    Host of your postgresql database.
  -p [ --port ] arg (=5432)         db_port of your database.
  -W [ --password ] arg             Password for database access.

```
