# osm2pgrouting

## Prerequisites

Before you can use this tool for importing [Openstreetmap](http://openstreetmap.org/) data, you will need to have:

1. postgresql
2. postgis
3. pgrouting
4. boost
5. expat
6. cmake

To prepare a database see the documentation of the [pgrouting website](http://pgrouting.postlbs.org/).

## Installation

For compiling this tool, you will need cmake. Then just type the following in the root directory:

    $ cmake .
    $ make

## Running

Start the program like this:

    $ ./bin/osm2pgrouting -file your-OSM-XML-File.osm -conf mapconfig.xml -dbname routing -user postgres -clean

All available parameters are:

required:

    -file <file>      -- name of your osm xml file
    -dbname <dbname>  -- name of your database
    -user <user>      -- name of the user, which have write access to the database
    -conf <file>      -- name of your configuration xml file

optional:

    -host <host>      -- host of your postgresql database (default: 127.0.0.1)
    -port <port>      -- port of your database (default: 5432)
    -passwd <passwd>  -- password for database access
    -clean            -- drop previously created tables
    -skipnodes        -- do not load nodes into the node table (reduces runtime)
