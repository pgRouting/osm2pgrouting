/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef SRC_EXPORT2DB_H_
#define SRC_EXPORT2DB_H_

#include <pqxx/pqxx>
#include <libpq-fe.h>
#include <map>
#include <vector>
#include <string>

#include "osm_elements/Node.h"
#include "osm_elements/Way.h"
#include "osm_elements/Relation.h"
#if 0
#include "configuration/Tag_key.h"
#include "configuration/Class.h"
#endif
#include "configuration/configuration.h"
#include "utilities/prog_options.h"
#include "database/table_management.h"

namespace osm2pgr {

/**
 * This class connects to a postgresql database. For using this class,
 * you also need to install postgis and pgrouting
 */

class Export2DB {
 public:
     typedef std::vector<Node> Nodes;
     typedef std::vector<Way> Ways;
     typedef std::vector<Relation> Relations;
     /**
      * Constructor 
      * @param vm variable map holding the configuration
      *
      */
     explicit Export2DB(const po::variables_map &p_vm, const std::string &db_conn);

     /**
      * Destructor
      * closes the connection to the database
      */
     ~Export2DB();

     //! connects to database
     int connect();
     bool has_postGIS() const;
     bool has_hstore() const;
     bool install_postGIS() const;

     //! creates needed tables and geometries
     void createTables() const;
     void createTempTables() const;


     /** @brief export values to osm_* table
      *
      * T must have:
      *     T.values
      *
      * @params[in] items  vector of values to be inserted into
      * @params[in] table 
      */
     template <typename T>
         void export_osm (
                 std::vector<T> &items,
                 const std::string &osm_name) const {
             auto osm_table = m_tables.get_table(osm_name);
             std::vector<std::string> values(items.size(), "");

             size_t i(0);
             for (auto it = items.begin(); it != items.end(); ++it, ++i) {
                 auto item = *it;
                 if (m_vm.count("hstore")) {
                     values[i] = tab_separated(item.values(osm_table.columns(), true));
                 } else {
                     values[i] = tab_separated(item.values(osm_table.columns(), false));
                 }
             }

             export_osm(values, osm_table);
         }

     void export_configuration(
             const std::map<std::string, Tag_key>& items) const;

     void exportWays(
             const Ways &ways,
             const Configuration &config) const;

     //! Be careful! It deletes the created tables!
     void dropTables() const;
     void createFKeys();

 private:

     void export_osm(
             const std::vector<std::string> &values,
             const Table &table) const;

     void process_section(const std::string &ways_columns, pqxx::work &Xaction) const;


     void dropTable(const std::string &table, pqxx::work &Xaction) const;
     bool createTempTable(
             const std::string &sql,
             const std::string &table);
     bool createTable(
             const std::string &sql,
             const std::string &table,
             const std::string &constraint = std::string("")) const;
     void addTempGeometry(
             const std::string &table,
             const std::string &geometry_type,
             pqxx::work &Xaction) const;
     void addTempGeometry(
             const std::string &table,
             const std::string &geometry_type) const;
     void addGeometry(
             const std::string &schema,
             const std::string &table,
             const std::string &geometry_type) const;
     void create_gindex(
             const std::string &index,
             const std::string &table) const;
     void create_idindex(
             const std::string &colname,
             const std::string &table) const;

     inline std::string full_table_name(const std::string &table) const {
         return tables_prefix + table + tables_suffix;
     }

     inline std::string addSchema(const std::string &table) const {
         return  (default_tables_schema() == "" ? ""
                 : default_tables_schema() + ".") + table;
     }
     inline std::string default_tables_schema() const {
         return tables_schema;
     }
     void fill_vertices_table(
             const std::string &table,
             const std::string &vertices_tab,
             pqxx::work &Xaction) const;
     void fill_source_target(
             const std::string &table,
             const std::string &vertices_tab,
             pqxx::work &Xaction) const;

 private:
#if 1
     PGconn *mycon;
#endif
#if 1
     mutable pqxx::connection db_conn;
#endif
     po::variables_map m_vm;

     std::string conninf;
     std::string tables_schema;
     std::string tables_prefix;
     std::string tables_suffix;

     std::string create_ways;
     std::string create_vertices;

     Tables m_tables;

};
}  // namespace osm2pgr

#endif  // SRC_EXPORT2DB_H_
