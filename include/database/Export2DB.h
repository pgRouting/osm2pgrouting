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
      * @param db_conn conection string 
      *
      */
     explicit Export2DB(const po::variables_map &vm, const std::string &db_conn);

     /**
      * Destructor
      * closes the connection to the database
      */
     ~Export2DB();

#if 1
     //! connects to database
     int connect();
#endif

     bool has_extension(const std::string &name) const;
#ifndef NDBEUG
     bool install_postGIS() const;
#endif

     //! creates needed tables and geometries
     void createTables() const;


     /** @brief export values to osm_* table
      *
      * T must have:
      *     T.values
      *
      * @param[in] items  vector of values to be inserted into
      * @param[in] table 
      */
     template <typename T>
         void export_osm (
                 std::vector<T> &items,
                 const std::string &table) const {
             auto osm_table = m_tables.get_table(table);
             std::vector<std::string> values(items.size(), "");

             size_t i(0);
             for (auto it = items.begin(); it != items.end(); ++it, ++i) {
                 auto item = *it;
                 values[i] = tab_separated(item.values(osm_table.columns(), true));
             }

             export_osm(values, osm_table);
         }

     void export_configuration(
             const std::map<std::string, Tag_key>& items) const;

     void exportWays(
             const Ways &ways,
             const Configuration &config) const;

     void dropTables() const;
     void createFKeys() const;
     void process_pois() const;
     bool exists(const std::string &table) const;

 private:

     void export_osm(
             const std::vector<std::string> &values,
             const Table &table) const;

     void process_section(const std::string &ways_columns, pqxx::work &Xaction) const;

     void fill_vertices_table(
             const std::string &table,
             const std::string &vertices_tab,
             pqxx::work &Xaction) const;

     void fill_source_target(
             const std::string &table,
             const std::string &vertices_tab,
             pqxx::work &Xaction) const;

     int64_t get_val(const std::string sql) const;
     void execute(const std::string sql) const;

     Table configuration() const {return m_tables.configuration();}
     Table vertices() const {return m_tables.vertices();}
     Table ways() const {return m_tables.ways();}
     Table pois() const {return m_tables.pois();}
     Table osm_ways() const {return m_tables.osm_ways();}
     Table osm_nodes() const {return m_tables.osm_nodes();}
     Table osm_relations() const {return m_tables.osm_relations();}

 private:
     po::variables_map m_vm;

     std::string conninf;

     Tables m_tables;

};
}  // namespace osm2pgr

#endif  // SRC_EXPORT2DB_H_
