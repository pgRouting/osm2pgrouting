#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"
#include <string>

namespace osm2pgr {


/*
 * configuring TABLE osm_nodes
 */ 


Table
Tables::ways_vertices_pgr_config() const {
    Table table(
            /* name */
            "ways_vertices_pgr",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            std::string(
                m_vm["prefix"].as<std::string>()
                + "ways"
                + m_vm["suffix"].as<std::string>()
                + "_vertices_pgr"),

            /* standard column creation string */
            std::string(
                " id bigserial"
                ", osm_id bigint"
                ", eout integer"
                ", lon decimal(11,8)"
                ", lat decimal(11,8)"
                ", cnt integer"
                ", chk integer"
                ", ein integer"
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
                "POINT");

            return table;
}


} //namespace osm2pgr
