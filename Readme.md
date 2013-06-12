# osm2pgrouting

## Requirements

Before you can use this tool for importing Openstreetmap data you need to install:

1. postgresl
2. postgis
3. pgrouting
4. boost
5. expat

and to prepare a database.

## Documentation

See in the documentation of the pgrouting website for more informations: http://pgrouting.org


For compiling this tool, you will need boost, libpq and expat:
Then just type: make

## How to use

Start the program like this:

```
./osm2pgrouting -file your-OSM-XML-File.osm -conf mapconfig.xml -dbname routing -user postgres -clean
```

A complete list of arguments are:

```
required: 
-file <file>  			-- name of your osm xml file
-dbname <dbname> 		-- name of your database
-user <user> 			-- name of the user, which have write access to the database
-conf <file> 			-- name of your configuration xml file

optional:
-host <host>  			-- host of your postgresql database (default: 127.0.0.1)
-port <port> 			-- port of your database (default: 5432)
-prefixtables <prefix> 	-- add at the beginning of table names
-passwd <passwd> 		-- password for database access
-clean 					-- drop previously created tables
-skipnodes				-- do not load nodes into the node table (reduces runtime) 
```
