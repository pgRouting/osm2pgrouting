#pragma once
#include <string>
#include "utilities/prog_options.h"

namespace osm2pgr {

class Table {
 public:
     Table() = default;
     Table(const Table &) = default;
     Table(
             const std::string &schema,
             const std::string &name,
             const std::string &full_name,

             const std::string &create_str,
             const std::string &other_columns,
             const std::string &geometry
          );

     void set_columns(const std::vector<std::string> &columns);

     /** @brief prefixNameSufix */
     inline std::string table_name() const {
         return m_full_name;
     }


     /** @brief schema.prefixNameSufix
      *
      * schema.prefixNameSufix
      * OR
      * prefixNameSufix
      *
      */
     std::string addSchema() const;
     std::string temp_name() const;
     std::string name() const {return m_name;};
     std::string full_name() const {return m_full_name;};

     inline std::vector<std::string> columns() const {
         return m_columns;
     }


     std::string tmp_create() const;
     std::string create() const;
     std::string drop() const;

 private:
     std::string m_name;
     std::string m_schema;
     std::string m_full_name;

     std::string m_create;
     std::string m_other_columns;
     std::string m_constraint;
     std::string m_geometry;
     std::vector<std::string> m_columns;
};




class Tables {
    public:
        Tables(const po::variables_map &vm);
        const Table& get_table(const std::string &name) const {
            if (name == "osm_nodes") return osm_nodes;
            else if (name == "osm_ways") return osm_ways;
            else if (name == "osm_relations") return osm_relations;
            else if (name == "configuration") return configuration;
            else if (name == "pointsofinterest") return points_of_interest;
            else if (name == "ways") return ways;
            else return ways_vertices_pgr;
        }

        std::string  post_process(const Table &table) const;



        po::variables_map m_vm;
        /*
         * Conpulsory tables
         */
        Table ways;
        Table ways_vertices_pgr;
        Table points_of_interest;
        Table configuration;

        /*
         * Optional tables
         */
        Table osm_nodes;
        Table osm_ways;
        Table osm_relations;

    private:
        Table osm_nodes_config() const;
        Table pois_config() const;
        Table osm_ways_config() const;
        Table osm_relations_config() const;
        Table configuration_config() const;
        Table ways_config() const;
        Table ways_vertices_pgr_config() const;
};

}


