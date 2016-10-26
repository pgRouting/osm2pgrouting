#pragma once
#include <string>
#include "utilities/prog_options.h"

namespace osm2pgr {

class Table {
 public:
    Table(
            const std::string &schema,
            const std::string &prefix,
            const std::string &name,
            const std::string &sufix,
            const std::string &create_str,
            const std::string &other_columns,
            const std::string &constraint,
            const std::string &geometry
         );
    void set_columns(const std::vector<std::string> &columns);

    /** @brief prefixNameSufix */
    inline std::string table_name() const {
        return m_prefix + m_name + m_suffix;
    }


    /** @brief schema.prefixNameSufix
     *
     * schema.prefixNameSufix
     * OR
     * prefixNameSufix
     *
     */
    inline std::string addSchema() const {
        return
            m_schema
            + (m_schema == "" ? "" :  ".")
            + table_name();
    }

    std::string temp_name() const;

    inline std::vector<std::string> columns() const {
        return m_columns;
    }


    std::string tmp_create() const;
    std::string create() const;
    std::string drop() const;

    private:
    std::string m_schema;
    std::string m_prefix;
    std::string m_name;
    std::string m_suffix;
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
            if (name == "osm_ways") return osm_ways;
            if (name == "osm_relations") return osm_relations;
            return osm_nodes;
        }



        po::variables_map m_vm;
        Table osm_nodes;
        Table osm_ways;
        Table osm_relations;
};

}


