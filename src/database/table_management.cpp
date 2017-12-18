/*PGR-GNU*****************************************************************

 Copyright (c) 2017 pgRouting developers
 Mail: project@pgrouting.org

 ------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
********************************************************************PGR-GNU*/


#include <unistd.h>

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

std::string
Table::gist_index() const {
    return "CREATE INDEX ON " + addSchema()
        + "\n  USING GIST (the_geom);";
}


std::string
Table::foreign_key(
        const std::string &column,
        const Table &table,
        const std::string &table_column) const {
    return "ALTER TABLE " + addSchema()
        + "\n  ADD FOREIGN KEY (" + column + ")"
        + "\n  REFERENCES " + table.addSchema() + "(" + table_column + ")"
        + "\n  ON UPDATE NO ACTION \n  ON DELETE NO ACTION;";
}


std::string
Table::unique(const std::string &column) const {
    return std::string("ALTER TABLE " + addSchema()
        + "\n  ADD UNIQUE (" + column + ")");
}

std::string
Table::primary_key(const std::string &column) const {
    return std::string("ALTER TABLE " + addSchema()
        + "\n  ADD PRIMARY KEY (" + column + ")");
}



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
        + m_constraint + ")";

    sql += " WITH (autovacuum_enabled = false);";

    if (m_geometry != "") {
        sql += "SELECT AddGeometryColumn('"
            + m_schema
            + (m_schema == "" ? "" : "', '")
            + table_name() + "', 'the_geom', 4326, '" + m_geometry + "', 2);";

        if (name() == "pointsofinterest") {
            sql += "SELECT AddGeometryColumn('"
                + m_schema
                + (m_schema == "" ? "" : "', '")
                + table_name() + "', 'new_geom', 4326, '" + m_geometry + "', 2);";
        }
    }


    return sql;
}


std::string
Table::drop() const {
    return "DROP TABLE IF EXISTS " + addSchema() + " CASCADE;";
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
                " FROM  " + configuration().temp_name() + " a LEFT JOIN  " + configuration().addSchema() + " b USING (tag_id) WHERE (b.tag_id IS NULL))"

                + " INSERT INTO "  +  configuration().addSchema() 
                + "(" + comma_separated(configuration().columns()) + ") "
                + " (SELECT " + comma_separated(configuration().columns()) + " FROM data); ");
        return str;
    }
    return "";
}



Tables::Tables(const  po::variables_map &vm) :
    m_vm(vm),
    /*
     * initializing tables
     */
    m_ways(ways_config()),
    m_ways_vertices_pgr(ways_vertices_pgr_config()),
    m_points_of_interest(pois_config()),
    m_configuration(configuration_config()),

    m_osm_nodes(osm_nodes_config()),
    m_osm_ways(osm_ways_config()),
    m_osm_relations(osm_relations_config())
{
    auto m_schema(vm["schema"].as<string>());
    m_schema += (m_schema == "" ? "" :  ".");
    m_points_of_interest.add_sql(
            "\nCREATE OR REPLACE FUNCTION "
            + m_schema
            + "osm2pgr_pois_update_part_of_topology()"
            "\nRETURNS BIGINT AS"
            "\n$$"
            "\n-----------------------------------------------------------------"
            "\n-- When the point of interest is part of the routing topology"
            "\n--  Sets the vid value to the corresponding id of the vertices table"
            "\n--  The distance from the routing topólogy to the point of Interest is 0"
            "\n-----------------------------------------------------------------\n"
            "\nDECLARE"
            "\n  curr_tot BIGINT;"
            "\nBEGIN"
              "\n  UPDATE " +  pois().addSchema() + " AS pois"
            + "\n      SET (vertex_id, length_m) = (vertices.id, 0)"
            + "\n    FROM " + vertices().addSchema() + " AS vertices"
            + "\n    WHERE vertex_id IS NULL AND pois.osm_id = vertices.osm_id;"
            + "\n  GET DIAGNOSTICS curr_tot = ROW_COUNT;"
            + "\n  RETURN curr_tot;"
            + "\nEND;"
            + "\n$$"
            + "\nLANGUAGE plpgsql;"
            + "\nCOMMENT ON FUNCTION " + m_schema + "osm2pgr_pois_update_part_of_topology()"
            + "\n  IS 'osm2pgrouting generated function';"
            );


    m_points_of_interest.add_sql(
            "CREATE OR REPLACE FUNCTION "
            + m_schema
            +"osm2pgr_pois_update_not_part_of_topology(radius FLOAT, within FLOAT, tooFar BIGINT[])"
            "\n RETURNS BIGINT AS"
            "\n $$"
            "\n-----------------------------------------------------------------"
            "\n-- When A point of interest is NOT part of the routing topology"
            "\n--  Looks for a POI that has not being assigned a vid or an edge_id"
            "\n--  - grabs all the POIS that are within the (radius) mts distance of the POI"
            "\n--  - grabs all the edges that are within the (radius + within)mts of the POI"
            "\n--  - FOR each POI in POIS"
            "\n--    - For closest edge to the POI:"
            "\n--      - if the closest point in the edge to the POI is the an ending vertex:"
            "\n--        - Sets the vid value to the corresponding id of the vertices table"
            "\n--      - if the closest point in the edge to the POI is NOT an ending vertex:"
            "\n--        - Sets the edge_id and the fraction values"
            "\n-----------------------------------------------------------------"
            "\n DECLARE"
            "\n    curr_tot BIGINT;"
            "\n BEGIN"
            "\n        WITH "
            "\n        poi AS ("
            "\n            SELECT ST_buffer(the_geom::geography, $1)::geometry AS bufferPois,"
            "\n            ST_buffer(the_geom::geography, $1 + $2)::geometry AS bufferWays"
            "\n            FROM " + pois().addSchema()
            +"\n            WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n            AND pid not in (SELECT unnest(tooFar))"
            +"\n            limit 1"
            +"\n        ),"
            +"\n        pois AS ("
            +"\n            SELECT * FROM " + pois().addSchema() + ", poi"
            +"\n            WHERE ST_Within(the_geom, bufferPois) "
            +"\n            AND vertex_id IS NULL AND edge_id IS NULL"
            +"\n            AND pid not in (SELECT unnest(tooFar))"
            +"\n        ),"
            +"\n        wayss AS ("
            +"\n            SELECT * FROM " + ways().addSchema() + ", poi"
            +"\n            WHERE ST_Intersects(the_geom, bufferWays)"
            +"\n        ),"
            +"\n        first AS ("
            +"\n            SELECT   ways.gid AS wid,"
            +"\n            source_osm, target_osm,"
            +"\n            ST_distance(pois.the_geom::geography,   ways.the_geom::geography) AS dist,"
            +"\n            pois.osm_id AS vid,"
            +"\n            ST_linelocatepoint(ways.the_geom, pois.the_geom) AS fraction"
            +"\n            FROM  wayss AS ways , pois"
            +"\n            WHERE pois.vertex_id IS NULL AND pois.edge_id IS NULL"
            +"\n        ),"

            +"\n        second AS ("
            +"\n            SELECT  vid, min(dist) FROM first group by vid"
            +"\n       ),"


            +"\n        third  AS ("
            +"\n            SELECT first.vid, NULL::bigint AS wid, NULL::FLOAT AS fraction, first.dist, source_osm AS v_osm_id FROM first, second WHERE dist = min AND fraction in (0)"
            +"\n            UNION "
            +"\n            SELECT first.vid, NULL::bigint AS wid, NULL::FLOAT AS fraction, first.dist, target_osm AS v_osm_id FROM first, second WHERE dist = min AND fraction in (1)"
            +"\n        ),"
            +"\n        last AS ("
            +"\n            SELECT third.*, b.id  FROM third join " +  vertices().addSchema() + " AS b ON (third.v_osm_id = b.osm_id)"
            +"\n            UNION"
            +"\n            SELECT first.vid, first.wid, first.fraction, first.dist, NULL AS v_osm_id, NULL::bigint AS id FROM first, second WHERE dist = min AND fraction not in (0, 1)"
            +"\n        )"

            +"\n        UPDATE " + pois().addSchema() +" AS pois   SET (vertex_id, edge_id, fraction, length_m) = (last.id, last.wid, last.fraction, last.dist)"
            +"\n        FROM last WHERE  pois.osm_id = last.vid;"
            +"\n        GET DIAGNOSTICS curr_tot = ROW_COUNT;"
            +"\n    return curr_tot;"
            +"\n END;"
            +"\n $$"
            +"\n LANGUAGE plpgsql;"
            +"\nCOMMENT ON FUNCTION " + m_schema + "osm2pgr_pois_update_not_part_of_topology(float,float,bigint[])"
            + "\n  IS 'osm2pgrouting generated function';"
            );

    m_points_of_interest.add_sql(
            "CREATE OR REPLACE FUNCTION "
            + m_schema
            +"osm2pgr_pois_find_side()"
            "\n RETURNS VOID AS"
            "\n $$"
            "\n WITH "
            "\n base AS ("
            "\n     SELECT pid, w.gid AS wid, w.the_geom AS wgeom, p.the_geom AS pgeom"
            "\n     FROM " + pois().addSchema() + " AS p JOIN " + ways().addSchema() + " AS w ON (edge_id = w.gid)"
            + "\n     WHERE edge_id IS NOT NULL AND side IS NULL"
            + "\n ),"

            + "\n foo AS ("
            + "\n     SELECT wid, ST_dumppoints(wgeom) AS dp"
            + "\n     FROM base"
            + "\n ),"

            + "\n blade AS ("
            + "\n     SELECT wid, ST_collect((dp).geom) AS blade"
            + "\n     FROM foo"
            + "\n     GROUP BY wid"
            + "\n ),"

            + "\n split AS ("
            + "\n     SELECT base.*, (ST_Dump(ST_split(wgeom, blade))).geom AS line"
            + "\n     FROM blade JOIN base"
            + "\n     USING (wid)"
            + "\n ),"

            + "\n distance AS ("
            + "\n     SELECT split.*, ST_distance(line, pgeom) AS dist FROM split"
            + "\n ),"

            + "\n second AS ("
            + "\n     SELECT  pid, min(dist) FROM distance GROUP BY pid"
            + "\n ), "

            + "\n last AS ("
            + "\n     SELECT pid,"
            + "\n         (ST_y(ST_startpoint(line)) - ST_y(ST_endpoint(line))) * ST_x(pgeom)"
            + "\n      +  (ST_x(ST_endpoint(line)) - ST_x(ST_startpoint(line))) * ST_y(pgeom)"
            + "\n      +  (ST_x(ST_startpoint(line)) * ST_y(ST_endpoint(line)) - ST_x(ST_endpoint(line))"
            + "\n      * ST_y(ST_startpoint(line))) AS val"
            + "\n     FROM distance join second using (pid) where dist = min"
            + "\n )"

            + "\n UPDATE " + pois().addSchema() + " set side = case when val>0 then 'L' when val<0 then 'R' else 'B' end "
            + "\n FROM last "
            + "\n WHERE last.pid = " + pois().addSchema() + ".pid;"
            +"\n $$"
            +"\n LANGUAGE sql;"
            +"\nCOMMENT ON FUNCTION " + m_schema + "osm2pgr_pois_find_side()"
            + "\n  IS 'osm2pgrouting generated function';"
            );

    m_points_of_interest.add_sql(
            "CREATE OR REPLACE FUNCTION "
            + m_schema
            +"osm2pgr_pois_new_geom()"
            "\n RETURNS VOID AS"
            "\n $$"
            "\n UPDATE " + pois().addSchema()
            + "\n     SET new_geom = ST_LineInterpolatePoint(e.the_geom, fraction)"
            + "\n         FROM " + ways().addSchema() + " AS e WHERE edge_id = gid;"

            "\n UPDATE " + pois().addSchema()
            + "\n     SET new_geom = the_geom"
            + "\n         WHERE vertex_id IS NOT NULL;"
            + "\n $$"
            + "\n LANGUAGE sql;"
            +"\nCOMMENT ON FUNCTION " + m_schema + "osm2pgr_pois_new_geom()"
            + "\n  IS 'osm2pgrouting generated function';"
            );


    m_points_of_interest.add_sql(
            "\nCREATE OR REPLACE FUNCTION "
            + m_schema
            +"osm2pgr_pois_update(radius FLOAT DEFAULT 200, within FLOAT DEFAULT 50)"
            "\n RETURNS BIGINT AS"
            "\n $$"
            "\n-----------------------------------------------------------------"
            "\n-- Cycles thru all the POIS to either:"
            "\n--  Sets the vid value to the corresponding id of the vertices table"
            "\n--  Set the edge_id & disance"
            "\n--  Sets the side"
            "\n-- "
            "\n-- By working on areas of (radius)mts on POIS"
            "\n-- looking on edges that are at least (within) mts of each POI"
            "\n-- POIS that do not have a closest edge is considered as tooFar"
            "\n-- "
            "\n-- TooFar: are the POIS that do not have a closest edge within (radius + within)mts"
            "\n-- Recommended values radius = 200, within 50 mts"
            "\n-----------------------------------------------------------------\n"
            "\n DECLARE"
            "\n    curr_tot BIGINT;"
            "\n    total BIGINT :=0;"
            "\n    rec RECORD;"
            "\n    factor FLOAT = 0.5;"
            "\n    tooFar BIGINT[];"
            "\n BEGIN"
            "\n    total = " + m_schema + "osm2pgr_pois_update_part_of_topology();"

            "\n    SELECT count(*) FROM " + pois().addSchema()
            +"\n    WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n    INTO rec; "

            +"\n    FOR i IN 1..rec.count LOOP"
            +"\n        curr_tot = " + m_schema + "osm2pgr_pois_update_not_part_of_topology(radius, within, tooFar);"

            +"\n        RAISE NOTICE '%: Updated % points of Interest', i, curr_tot;"
            +"\n        total := total + curr_tot;"
            +"\n        IF (curr_tot = 0) THEN"
            +"\n            SELECT pid FROM " + pois().addSchema()
            +"\n                WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n                AND pid not in (SELECT unnest(tooFar))"
            +"\n                limit 1 INTO rec;"
            +"\n            raise notice 'Not within range: pid = %', rec.pid;"
            +"\n            tooFar := tooFar || rec.pid;"
            +"\n            SELECT count(*) FROM (SELECT * FROM " + pois().addSchema()
            +"\n                WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n                AND pid not in (SELECT unnest(tooFar)) LIMIT 1) a  INTO rec;"

            +"\n            EXIT WHEN rec.count = 0;"
            +"\n        END IF;"
            +"\n    END LOOP;"
            +"\n    PERFORM " + m_schema + "osm2pgr_pois_find_side();"
            +"\n    PERFORM " + m_schema + "osm2pgr_pois_new_geom();"

            +"\n    return total;"
            +"\n END;"
            +"\n $$"
            +"\n LANGUAGE plpgsql;"
            +"\nCOMMENT ON FUNCTION " + m_schema + "osm2pgr_pois_update(float, float)"
            + "\n  IS 'osm2pgrouting generated function. " + m_schema + "osm2pgr_pois_update(radius, within)\nworking on areas of (radius)mts\nOn edges that are at least (within) mts of each POI';"
            );


}


} // namespace osm2pgr
