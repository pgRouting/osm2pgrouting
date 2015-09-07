/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef EXPORT2DB_H
#define EXPORT2DB_H

#if 1  // Nagase San code
#include "libpq-fe.h"
#else  // Code we were using temporarly
#ifdef __MINGW32__
#include "libpq-fe.h"
#else
#include "postgresql/libpq-fe.h"
#endif
#endif

#include "Node.h"
#include "Way.h"
#include "Relation.h"
#include "Type.h"
#include "Class.h"
#include "Configuration.h"
#include "prog_options.h"
 
using namespace osm;

/**
 * This class connects to a postgresql database. For using this class,
 * you also need to install postgis and pgrouting
 */

class Export2DB
{
public:
	/**
	 * Construktor
	 * @param host Host address of the database
	 * @param user a user, who has write access to the database
	 * @param dbname name of the database
	 *
	 */
 	Export2DB(const  po::variables_map &vm);
 	
 	/**
 	 * Destructor
 	 * closes the connection to the database
 	 */
 	~Export2DB();

 	//! connects to database
 	int connect();

 	//! creates needed tables and geometries
 	void createTables() const;
 	void createTempTables() const;
 	//! exports nodes to the database
 	void exportNodes(const std::map<long long, Node*>& nodes) const;
 	//! exports ways to the database
 	void exportTags(const std::vector<Way*> &ways, Configuration *config) const;
 	void exportRelations(const std::vector<Relation*> &relations, Configuration *config) const;
 	void exportRelationsWays(const std::vector<Relation*> &relations, Configuration *config) const;
 	void exportTypes(const std::map<std::string, Type*>& types) const;
 	void exportClasses(const std::map<std::string, Type*>& types) const;
 	void exportWays(const std::vector<Way*> &ways, Configuration *config) const;

 	/**
 	 * creates the topology
 	 * Be careful, it takes some time.
 	 *
 	 * for example:
 	 * complete germany: OSM file with a size of 1,1 GiB.
 	 * Export and create topology:
 	 * time took circa 30 hours on an Intel Xeon 2,4 GHz with 2 GiB Ram.
 	 * But only for the streettypes "motorway", "primary" and "secondary"
 	 */
 	void createTopology() const;
 	//! Be careful! It deletes the created tables!
 	void dropTables() const;
 	void dropTempTables() const;

 private:
    //! to use with creating the ways
    void prepare_table(const std::string &ways_columns) const;
    void process_section(int count, const std::string &ways_columns) const;

    void dropTempTable(const std::string &table) const;
    bool createTempTable(const std::string &sql,
			 const std::string &table) const;
    void dropTable(const std::string &table) const;
    bool createTempTable(const std::string &sql,
			 const std::string &table);
    bool createTable(const std::string &sql,
			 const std::string &table,
			 const std::string &constraint = std::string("")) const;
    void addTempGeometry( const std::string &table,
                         const std::string &geometry_type) const;
    void addGeometry( const std::string &schema, const std::string &table,
                         const std::string &geometry_type) const;
    void create_gindex(const std::string &index, const std::string &table) const;
    void create_idindex(const std::string &colname, const std::string &table) const;
    inline std::string full_table_name(const std::string &table) const {
		return tables_prefix + table + tables_suffix;
        }
    inline std::string addSchema(const std::string &table) const {
        return  (default_tables_schema() == "" ? "" : default_tables_schema() + ".") + table;
        }
    inline std::string default_tables_schema() const {
		return tables_schema;
        }
	void fill_vertices_table(const std::string &table, const std::string &vertices_tab) const;
	void fill_source_target(const std::string &table, const std::string &vertices_tab) const;

private:
	PGconn *mycon;
	std::string conninf;
	std::string tables_schema;
	std::string tables_prefix;
	std::string tables_suffix;

        // create table query constants
	std::string create_classes;
	std::string create_nodes;
	std::string create_ways;
	std::string create_relations;
	std::string create_relations_ways;
	std::string create_way_tag;
	std::string create_types;
	std::string create_vertices;
};

#endif
