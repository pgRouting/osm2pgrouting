#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"

namespace osm2pgr {

Table::Table(
        const std::string &name,
        const std::string &schema,
        const std::string &full_name,

        const std::string &create_str,
        const std::string &other_columns,
        const std::string &geometry
        ):
    m_name(name),
    m_schema(schema),
    m_full_name(full_name),

    m_create(create_str),
    m_other_columns(other_columns),
    m_geometry(geometry)
    { }

void 
Table::set_columns(const std::vector<std::string> &columns) {
    m_columns = columns;
}

std::string
Table::addSchema() const {
    return
        m_schema
        + (m_schema == "" ? "" :  ".")
        + m_full_name;
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


std::string
Tables::post_process(const Table &table) const  {
    if (table.name() == "osm_nodes"
            || table.name() == "pointsofinterest"
            || table.name() == "osm_ways"
            || table.name() == "osm_relations") { 
        std::string str(
                " WITH data AS ("
                " SELECT a.* "
                " FROM  " + table.temp_name() + " a LEFT JOIN  " + table.addSchema() + " b USING (osm_id) WHERE (b.osm_id IS NULL))"

                + " INSERT INTO "  +  table.addSchema() 
                + "(" + comma_separated(table.columns()) + ") "
                + " (SELECT " + comma_separated(table.columns()) + " FROM data); ");
        return str;
    } else if (table.name() == "configuration") {

        std::string str(
                " WITH data AS ("
                " SELECT a.* "
                " FROM  " + configuration.temp_name() + " a LEFT JOIN  " + configuration.addSchema() + " b USING (tag_id) WHERE (b.tag_id IS NULL))"

                + " INSERT INTO "  +  configuration.addSchema() 
                + "(" + comma_separated(configuration.columns()) + ") "
                + " (SELECT " + comma_separated(configuration.columns()) + " FROM data); ");
        return str;
    }
    return "";
}



Tables::Tables(const  po::variables_map &vm) :
    m_vm(vm),
    /*
     * initializing tables
     */
    ways(ways_config()),
    ways_vertices_pgr(ways_vertices_pgr_config()),
    points_of_interest(pois_config()),
    configuration(configuration_config()),

    osm_nodes(osm_nodes_config()),
    osm_ways(osm_ways_config()),
    osm_relations(osm_relations_config())
{
}


} // namespace osm2pgr
