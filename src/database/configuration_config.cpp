#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"

namespace osm2pgr {


/*
 * configuring TABLE configuration
 */ 


Table
Tables::configuration_config() const {
    Table table(
            /* schema */
            m_vm["schema"].as<std::string>(),
            /* prefix */
            "",
            /* name */
            "configuration",
            /* suffix */
            "",
            /* standard column creation string */
            std::string(
                " id serial PRIMARY KEY"
                ", tag_id INTEGER"
                ", tag_key TEXT"
                ", tag_value TEXT"
                ", priority double precision"
                ", maxspeed double precision"
                ", maxspeed_forward double precision"
                ", maxspeed_backward double precision"
                ", force char"),
            /* other columns */
            "",
            /* constraint */
            "",
            /* geometry */
            "");



    std::vector<std::string> columns;
    columns.push_back("tag_id");
    columns.push_back("tag_key");
    columns.push_back("tag_value");
    columns.push_back("priority");
    columns.push_back("maxspeed");
    columns.push_back("maxspeed_forward");
    columns.push_back("maxspeed_backward");
    columns.push_back("force");

    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
