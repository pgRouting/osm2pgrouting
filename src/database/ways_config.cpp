#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"
#include <string>

namespace osm2pgr {


/*
 * configuring TABLE osm_nodes
 */ 


Table
Tables::ways_config() const {
    Table table(
            /* name */
            "ways",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            std::string(
                m_vm["prefix"].as<std::string>()
                + "ways"
                + m_vm["suffix"].as<std::string>()),

            /* standard column creation string */
            std::string(
                " gid bigserial PRIMARY KEY"
                ", class_id integer not null"
                ", length double precision"
                ", length_m double precision"
                ", name text"
                ", source bigint"
                ", target bigint"
                ", x1 double precision"
                ", y1 double precision"
                ", x2 double precision"
                ", y2 double precision"
                ", cost double precision"
                ", reverse_cost double precision"
                ", cost_s double precision "
                ", reverse_cost_s double precision"
                ", rule text"
                ", one_way int "
                ", maxspeed_forward double precision"
                ", maxspeed_backward double precision"
                ", osm_id bigint"
                ", source_osm bigint"
                ", target_osm bigint"
                ", priority double precision DEFAULT 1"
#if 0
                + (m_vm.count("attributes") ?
                        (std::string(", attributes ") + (m_vm.count("hstore") ? "hstore" : "json"))
                        : "")
                + (m_vm.count("tags") ?
                        (std::string(", tags ") + (m_vm.count("hstore") ? "hstore" : "json"))
                        : "")
#endif
                ),

            /* other columns */
            "",

            /* geometry */
            "LINESTRING");


    std::vector<std::string> columns;
    columns.push_back("class_id");
    columns.push_back("osm_id");
    columns.push_back("maxspeed_forward");
    columns.push_back("maxspeed_backward");
    columns.push_back("one_way");
    columns.push_back("priority");

    columns.push_back("length");
    columns.push_back("x1"); columns.push_back("y1");
    columns.push_back("x2"); columns.push_back("y2");
    columns.push_back("source_osm");
    columns.push_back("target_osm");
    columns.push_back("the_geom");
    columns.push_back("cost");
    columns.push_back("reverse_cost");
    columns.push_back("name");


#if 0
    // TODO get from the configuration
    columns.push_back("tag_name");
    columns.push_back("tag_value");

    // end todo
    if (m_vm.count("attributes")) columns.push_back("attributes");
    if (m_vm.count("tags")) columns.push_back("tags");
#endif
    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
