#include "database/table_management.h"
#include "utilities/utilities.h"
#include <vector>

namespace osm2pgr {


/*
 * configuring TABLE osm_ways
 */ 


Table
Tables::osm_ways_config() const {
    Table table(
            /* name */
            "osm_ways",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            "osm_ways",

            /* standard column creation string */
            std::string(
                " osm_id bigint PRIMARY KEY"
                " , members " +  (std::string(m_vm.count("hstore") ? "hstore" : "json"))
                + (m_vm.count("attributes") ?
                    (std::string(", attributes ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")
                + (m_vm.count("tags") ?
                    (std::string(", tags ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")),

            /* other columns */
            /* TODO get from the configuration maybe this task is to be done on the configuration*/
            std::string(
                    ", tag_name TEXT"
                    ", tag_value TEXT"
                    ", name TEXT "),
            // end todo

            /* geometry */
            "LINESTRING");


    std::vector<std::string> columns;
    columns.push_back("osm_id");
    columns.push_back("members");
    // TODO get from the configuration
    columns.push_back("tag_name");
    columns.push_back("tag_value");
    columns.push_back("name");
    // end todo
    if (m_vm.count("attributes")) columns.push_back("attributes");
    if (m_vm.count("tags")) columns.push_back("tags");
    columns.push_back("the_geom");

    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
