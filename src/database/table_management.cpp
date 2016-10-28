#include "boost/lexical_cast.hpp"
#include "database/table_management.h"

namespace osm2pgr {

Table::Table(
        const std::string &schema,
        const std::string &prefix,
        const std::string &name,
        const std::string &suffix,
        const std::string &create_str,
        const std::string &other_columns,
        const std::string &constraint,
        const std::string &geometry
        ):
    m_schema(schema),
    m_prefix(prefix),
    m_name(name),
    m_suffix(suffix),
    m_create(create_str),
    m_other_columns(other_columns),
    m_constraint(constraint),
    m_geometry(geometry)
    { }

void 
Table::set_columns(const std::vector<std::string> &columns) {
    m_columns = columns;
}


/*
 * sql strings
 */

std::string
Table::create() const {
    std::string sql =
        "CREATE TABLE " + addSchema() + " ("
        + m_create
        + m_other_columns
        + m_constraint + ");";
    if (m_geometry != "") {

        sql += "SELECT AddGeometryColumn('"
            + m_schema
            + (m_schema == "" ? "" : "', '")
            + table_name() + "', 'the_geom', 4326, '" + m_geometry + "', 2);";
    }
    return sql;
}

std::string
Table::drop() const {
    return "DROP TABLE IF EXISTS " + addSchema() + ";";
}

    
std::string
Table::temp_name() const {
    return
        "__" 
        + table_name()
        + boost::lexical_cast<std::string>(getpid());
}


std::string
Table::tmp_create() const {
    std::string sql =
        "CREATE UNLOGGED TABLE " 
        + temp_name()
        + " ("
        + m_create
        + m_other_columns
        + ");";
    if (m_geometry != "") {
        sql += "SELECT AddGeometryColumn('"
            + temp_name() + "', 'the_geom', 4326, '" + m_geometry + "', 2);";
    }
    return sql;
}




Tables::Tables(const  po::variables_map &vm) :
    m_vm(vm),
    /*
     * initializing tables
     */
    osm_nodes(
            /* schema */
            vm["schema"].as<std::string>(),
            /* prefix */
            "",
            /* name */
            "osm_nodes",
            /* suffix */
            "",
            /* standard column creation string */
            std::string(
                " osm_id bigint PRIMARY KEY"
                + (vm.count("attributes") ?
                    (std::string(", attributes ") + (vm.count("hstore") ? "hstore" : "json"))
                    : "")
                + (vm.count("tags") ?
                    (std::string(", tags ") + (vm.count("hstore") ? "hstore" : "json"))
                    : "")),
            /* other columns */
            // TODO get from the configuration maybe this task is to be done on the configuration*/
            ", tag_name TEXT"
            ", tag_value TEXT"
            ", name TEXT ",
            // end todo
            /* constraint */
            "",
            /* geometry */
            "POINT"),
     osm_ways(
             /* schema */
             vm["schema"].as<std::string>(),
             /* prefix */
             "",
             /* name */
             "osm_ways",
             /* suffix */
             "",
             /* standard column creation string */
             std::string(
                 " osm_id bigint PRIMARY KEY"
                 " , members " +  (std::string(vm.count("hstore") ? "hstore" : "json"))
                 + (vm.count("attributes") ?
                     (std::string(", attributes ") + (vm.count("hstore") ? "hstore" : "json"))
                     : "")
                 + (vm.count("tags") ?
                     (std::string(", tags ") + (vm.count("hstore") ? "hstore" : "json"))
                     : "")),
             /* other columns */
             // TODO get from the configuration maybe this task is to be done on the configuration*/
             ", tag_name TEXT"
             ", tag_value TEXT"
             ", name TEXT ",
             // end todo
             /* constraint */
             "",
             /* geometry */
             "LINESTRING"),
     osm_relations(
             /* schema */
             vm["schema"].as<std::string>(),
             /* prefix */
             "",
             /* name */
             "osm_relations",
             /* suffix */
             "",
             /* standard column creation string */
             std::string(
                 " osm_id bigint PRIMARY KEY"
                 " , members " +  (std::string(vm.count("hstore") ? "hstore" : "json"))
                 + (vm.count("attributes") ?
                     (std::string(", attributes ") + (vm.count("hstore") ? "hstore" : "json"))
                     : "")
                 + (vm.count("tags") ?
                     (std::string(", tags ") + (vm.count("hstore") ? "hstore" : "json"))
                     : "")
                 ),
             /* other columns */
             // TODO get from the configuration maybe this task is to be done on the configuration*/
             ", tag_name TEXT"
             ", tag_value TEXT"
             ", name TEXT ",
             // end todo
             /* constraint */
             "",
             /* geometry */
             ""),


     configuration(
             /* schema */
             vm["schema"].as<std::string>(),
             /* prefix */
             "",
             /* name */
             "configuration",
             /* suffix */
             "",
             /* standard column creation string */
             std::string(
             " id bigint PRIMARY KEY"
             ", tag_id INTEGER"
             ", tag_key TEXT"
             ", tag_value TEXT"
             ", priority double precision"
             ", maxspeed double precision"
             ", maxspeed_forward double precision"
             ", maxspeed_backward double precision"),
             /* other columns */
             "",
             /* constraint */
             "",
             /* geometry */
             "")
{
    {
        /*
         * configuring TABLE osm_nodes
         */ 
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
        osm_nodes.set_columns(columns);
    }

    {
        /*
         * configuring TABLE osm_ways
         */ 
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
        osm_ways.set_columns(columns);
    }

    {
        /*
         * configuring TABLE osm_relations
         */ 
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
        osm_relations.set_columns(columns);
    }

    {
        /*
         * configuring TABLE configuration
         */ 
        std::vector<std::string> columns;
        columns.push_back("tag_id");
        columns.push_back("tag_key");
        columns.push_back("tag_value");
        columns.push_back("priority");
        columns.push_back("maxspeed");
        columns.push_back("maxspeed_forward");
        columns.push_back("maxspeed_backward");
        configuration.set_columns(columns);
    }
}


} // namespace osm2pgr
