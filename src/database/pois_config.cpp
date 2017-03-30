#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"

namespace osm2pgr {


/*
 * configuring TABLE osm_nodes
 */ 


Table
Tables::pois_config() const {
    Table table(
            /* name */
            std::string("pointsofinterest"),

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            std::string(
                m_vm["prefix"].as<std::string>()
                + "pointsofinterest"
                + m_vm["suffix"].as<std::string>()),


            /* standard column creation string */
            std::string(
                "  pid bigserial"
                ", osm_id bigint"
                ", vertex_id bigint"
                ", edge_id bigint"
                ", side CHAR"
                ", fraction FLOAT"
                ", length_m FLOAT"
                + (m_vm.count("attributes") ?
                    (std::string(", attributes ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")
                + (m_vm.count("tags") ?
                    (std::string(", tags ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")),

            /* other columns */
            std::string(
                    ", tag_name TEXT"
                    ", tag_value TEXT"
                    ", name TEXT "),
            // end todo

            /* geometry */
            "POINT");
    std::vector<std::string> columns;
    columns.push_back("osm_id");
    columns.push_back("the_geom");
    // TODO get from the configuration
    columns.push_back("tag_name");
    columns.push_back("tag_value");
    columns.push_back("name");
    // end todo
    if (m_vm.count("attributes")) columns.push_back("attributes");
    if (m_vm.count("tags")) columns.push_back("tags");
    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
