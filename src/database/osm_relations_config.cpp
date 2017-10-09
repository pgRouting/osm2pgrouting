#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"

namespace osm2pgr {


/*
 * configuring TABLE osm_relations
 */ 


Table
Tables::osm_relations_config() const {
    Table table(
            /* name */
            "osm_relations",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            "osm_relations",

            /* standard column creation string */
            std::string(
                " osm_id bigint PRIMARY KEY"
                " , members hstore"
                + (m_vm.count("attributes") ?
                    (std::string(", attributes hstore"))
                    : "")
                + (m_vm.count("tags") ?
                    (std::string(", tags hstore"))
                    : "")
                ),
            /* other columns */
            // TODO get from the configuration maybe this task is to be done on the configuration*/
            ", tag_name TEXT"
                ", tag_value TEXT"
                ", name TEXT ",
            // end todo

            /* geometry */
            "");


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

    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
