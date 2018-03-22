# osm2pgrouting

[![Join the chat at https://gitter.im/pgRouting/osm2pgrouting](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/pgRouting/osm2pgrouting?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# Table of Contents

* [Requirements](#requirements)
* [Documentation](#documentation)
* [Installation](#installation)
* [How to use](#how-to-use)
* [Tips](#tips)
  

## Requirements

Before you can use this tool for importing Openstreetmap data you need to install:

1. postgresql
2. postgis
3. pgrouting
4. boost
5. expat
5. libpqxx
6. cmake

and to prepare a database.

## Documentation

See in the documentation of the pgrouting website for more information: http://pgrouting.org

## Installation

For compiling this tool, you will need boost, libpqxx, expat and cmake:
Then just type the following in the root directory:

```
cmake -H. -Bbuild
cd build/
make
make install
```

Install some prerequisites:

```
sudo apt-get install expat
sudo apt-get install libexpat1-dev
sudo apt-get install libboost-dev
sudo apt-get install libboost-program-options-dev
sudo apt install libpqxx-dev
```

**Note:** FindLibPQXX.cmake does not find the version of libpqxx, but its documentation says C++11 is needed for the latests versions.


If you have libraries installed in non-standard locations, you might need to pass in parameters to cmake. Commonly useful parameters are

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
psql --dbname routing -c 'CREATE EXTENSION postgis'
psql --dbname routing -c 'CREATE EXTENSION pgRouting'
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
 osm2pgrouting --help
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
  --postgis                             Install postgis if not found.
  --addnodes                            Import the osm_nodes, osm_ways &
                                        osm_relations tables.
  --attributes                          Include attributes information.
  --tags                                Include tag information.
  --chunk arg (=20000)                  Exporting chunk size.
  --clean                               Drop previously created tables.
  --no-index                            Do not create indexes (Use when indexes
                                        are already created)

Database options:
  -d [ --dbname ] arg            Name of your database (Required).
  -U [ --username ] arg          Name of the user, which have write access to
                                 the database.
  -h [ --host ] arg (=localhost) Host of your postgresql database.
  -p [ --port ] arg (=5432)      db_port of your database.
  -W [ --password ] arg          Password for database access.

```

## Tips

Open Street Map (OSM) files contains tags not used at all for routing operations by PgRouting (i.e. author, version, timestamps, etc.). You can reduce a lot the size of your OSM file to import removing this metadata tags from original file (you can get around half size of original file).

The best tool to remove tags is [osmconvert](https://wiki.openstreetmap.org/wiki/Osmconvert).
There are another tools but osmconvert is the fastest parsing osm files. 

Example:
```
$ osmconvert output_data.osm.pbf --drop-author --drop-version --out-osm -o=output_data_reduc.osm
```

You can download OSM data as PBF (protobuffer) format. This is a binary format and it has a lower size than OSM raw files (better for downloading operations).
