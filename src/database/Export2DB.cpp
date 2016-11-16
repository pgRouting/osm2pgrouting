/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program IS free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License AS published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program IS distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "database/Export2DB.h"
#include "database/table_management.h"

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "utilities/print_progress.h"
#include "utilities/prog_options.h"
#include "utilities/utilities.h"

#include "boost/algorithm/string/replace.hpp"


namespace osm2pgr {

template <typename T>
static
std::string
TO_STR(const T &x) {
    return  boost::lexical_cast<std::string>(x);
}


Export2DB::Export2DB(const  po::variables_map &vm, const std::string &connection) :
    mycon(0),
    db_conn(connection),
    m_vm(vm),
    conninf(connection),
    m_tables(vm)
{
}

Export2DB::~Export2DB() {
    PQfinish(mycon);
}

int Export2DB::connect() {
    cout << conninf << endl;
    mycon = PQconnectdb(conninf.c_str());

    auto type = PQstatus(mycon);
    if (type == CONNECTION_BAD) {
        cout << "connection failed: "<< PQerrorMessage(mycon) << endl;
        return 1;
    } else {
        cout << "connection success"<< endl;
        return 0;
    }
}


bool
Export2DB::has_extension(const std::string &name) const {
    try {
        pqxx::work Xaction(db_conn);
        std::string sql = "SELECT * FROM pg_extension WHERE extname = '" + name + "'";
        auto result = Xaction.exec(sql);
        return result.size() == 1;

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return false;
    }
}


#ifndef NDEBUG
bool
Export2DB::install_postGIS() const {
    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec("CREATE EXTENSION postgis");
        Xaction.exec("CREATE EXTENSION hstore");
        Xaction.commit();
        return true;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}
#endif




// /////////////////////
void Export2DB::createTables() const {
    //  the following are particular of the file tables
    try {
        pqxx::work Xaction(db_conn);

        Xaction.exec(m_tables.ways_vertices_pgr.create());
        std::cout << "TABLE: " << m_tables.ways_vertices_pgr.addSchema() << " created ... OK.\n";

        Xaction.exec(m_tables.ways.create());
        std::cout << "TABLE: " << m_tables.ways.addSchema() << " created ... OK.\n";

        Xaction.exec(m_tables.points_of_interest.create());
        std::cout << "TABLE: " << m_tables.ways.addSchema() << " created ... OK.\n";

        Xaction.exec(m_tables.configuration.create());
        std::cout << "TABLE: " << m_tables.configuration.addSchema() << " created ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
        std::cerr <<  "FATAL ERROR: could not routing tables" << std::endl;
        exit(1);
    }

    try {
        pqxx::work Xaction(db_conn);
        /*
         * optional tables
         */
        Xaction.exec(m_tables.osm_nodes.create());
        std::cout << "TABLE: " << m_tables.osm_nodes.addSchema() << " created ... OK.\n";

        Xaction.exec(m_tables.osm_ways.create());
        std::cout << "TABLE: " << m_tables.osm_ways.addSchema() << " created ... OK.\n";

        Xaction.exec(m_tables.osm_relations.create());
        std::cout << "TABLE: " << m_tables.osm_relations.addSchema() << " created ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
        std::cerr <<  "WARNING: could not create osm-*  tables" << std::endl;
        std::cerr <<  "   Insertions on osm_* tables are going to be ignored" << std::endl;
    }
}




void Export2DB::dropTables() const {
    try {
        pqxx::work Xaction(db_conn);

        Xaction.exec(m_tables.ways.drop());
        std::cout << "TABLE: " << m_tables.ways.addSchema() << " droped ... OK.\n";

        Xaction.exec(m_tables.ways_vertices_pgr.drop());
        std::cout << "TABLE: " << m_tables.ways_vertices_pgr.addSchema() << " droped ... OK.\n";

        Xaction.exec(m_tables.configuration.drop());
        std::cout << "TABLE: " << m_tables.configuration.addSchema() << " droped ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        cerr << "ROLLBACK applied";
    }

    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec(m_tables.osm_nodes.drop());
        std::cout << "TABLE: " << m_tables.osm_nodes.addSchema() << " droped ... OK.\n";

        Xaction.exec(m_tables.osm_ways.drop());
        std::cout << "TABLE: " << m_tables.osm_ways.addSchema() << " droped ... OK.\n";

        Xaction.exec(m_tables.osm_relations.drop());
        std::cout << "TABLE: " << m_tables.osm_relations.addSchema() << " droped ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}


void
Export2DB::export_configuration(const std::map<std::string, Tag_key>& items) const {

    auto osm_table = m_tables.get_table("configuration");

    std::vector<std::string> values;

    for (const auto &item : items) {
        auto row = item.second.values(osm_table.columns());
        values.insert(values.end(), row.begin(), row.end());
    }

    export_osm(values, osm_table);
}


void
Export2DB::export_osm(
        const std::vector<std::string> &values,
        const Table &table) const {
    if (values.empty()) return;

    auto columns = table.columns();
    std::string temp_table(table.temp_name());
    auto create_sql = table.tmp_create();
    std::string copy_sql( "COPY " + temp_table + " (" + comma_separated(columns) + ") FROM STDIN");

#if 0
    std::cout << "\n" << create_sql;
    std::cout << "\n" << copy_sql;

#endif

    size_t count = 0;
    try {


        pqxx::connection db_con(conninf);
        pqxx::work Xaction(db_con);
        PGconn *mycon = PQconnectdb(conninf.c_str());

        PGresult *res = PQexec(mycon, create_sql.c_str());
        res = PQexec(mycon, copy_sql.c_str());

        for (auto it = values.begin(); it != values.end(); ++it) {
            auto str = *it;

            ++count;

            PQputline(mycon, str.c_str());
        }

        PQputline(mycon, "\\.\n");

        if (PQendcopy(mycon) != 0) {
            Xaction.exec("DROP TABLE " + temp_table);
            PQfinish(mycon);
            Xaction.commit();

            if (values.size() < 2) {
                for (const auto &v : values) {
                std::cout << "\n*****ERROR HERE:\n" << v << "\n******";
                }
                return;
            }
            size_t inc = values.size() / 2;
            export_osm(std::vector<std::string>(values.begin(), values.begin() + inc), table);
            export_osm(std::vector<std::string>(values.begin() + inc , values.end()), table);
            return;
        };

        Xaction.exec(m_tables.post_process(table));
        Xaction.exec("DROP TABLE " + temp_table);
        PQfinish(mycon);
        Xaction.commit();

    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
        std::cerr << "While exporting to " << table.addSchema() << " TODO insert one by one skip the guilty one\n";
    }
}




/*!

*/
void Export2DB::fill_vertices_table(
        const std::string &table,
        const std::string &vertices_tab,
        pqxx::work &Xaction) const {
    // std::cout << "Filling '" << vertices_tab << "' based on '" << table <<"'\n";
    std::string sql(
            "WITH osm_vertex AS ("
            "(SELECT source_osm AS osm_id, x1 AS lon, y1 AS lat FROM " + table + " where source IS NULL)"
            " union "
            "(SELECT target_osm AS osm_id, x2 AS lon, y2 AS lat FROM " + table + " where target IS NULL)"
            ") , "
            " data1 AS (SELECT osm_id, lon, lat FROM (SELECT DISTINCT * FROM osm_vertex) a "
            ") "
            " INSERT INTO " + vertices_tab + " (osm_id, lon, lat, the_geom) (SELECT data1.*, ST_SetSRID(ST_Point(lon, lat), 4326) FROM data1)");
    auto result = Xaction.exec(sql);

    std::cout << "\t Vertices inserted: " << result.affected_rows();
}





void Export2DB::fill_source_target(
        const std::string &table,
        const std::string &vertices_tab,
        pqxx::work &Xaction) const {
    // std::cout << "    Filling 'source' column of '" << table << "':'" << vertices_tab << "'\n";
    std::string sql1(
            " UPDATE " + table + " AS w"
            " SET source = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.source IS NULL and w.source_osm = v.osm_id;");
    Xaction.exec(sql1);

    std::string sql2(
            " UPDATE " + table + " AS w"
            " SET target = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.target IS NULL and w.target_osm = v.osm_id;");
    Xaction.exec(sql2);

    std::string sql3(
            " UPDATE " + table +
            " SET  length_m = ST_length(geography(ST_Transform(the_geom, 4326))),"
            "      coST_s = CASE "
            "           WHEN one_way = -1 THEN -ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_forward::float * 5.0 / 18.0)"
            "           ELSE ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END, "
            "      reverse_coST_s = CASE "
            "           WHEN one_way = 1 THEN -ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "           ELSE ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END "
            " WHERE length_m IS NULL;");
    Xaction.exec(sql3);
}




void Export2DB::exportWays(const Ways &ways, const Configuration &config) const {
    std::cout << "    Processing " <<  ways.size() <<  " ways"  << ":\n";

    Table table = m_tables.ways;

    auto columns = table.columns();
    auto ways_columns = comma_separated(columns);

    size_t chunck_size = m_vm["chunk"].as<size_t>();

    auto create_sql = table.tmp_create();
    auto temp_table(table.temp_name());

    std::string copy_sql( "COPY " + temp_table + " (" + comma_separated(columns) + ") FROM STDIN");


    int64_t split_count = 0;
    int64_t count = 0;
    size_t start = 0;
    auto it = ways.begin();

    while (start < ways.size()) {
        auto limit = (start + chunck_size) < ways.size() ? start + chunck_size : ways.size();
        try {
            pqxx::connection db_con(conninf);
            pqxx::work Xaction(db_con);

            PGconn *mycon = PQconnectdb(conninf.c_str());
            PGresult *res = PQexec(mycon, create_sql.c_str());
            res = PQexec(mycon, copy_sql.c_str());


            for (auto i = start; i < limit; ++i) {
                auto way = *it;

                ++count;
                ++it;

                if (way.tag_config().key() == "" || way.tag_config().value() == "") continue;

                std::vector<std::string> common_values;
                common_values.push_back(TO_STR(config.tag_value(way.tag_config()).id()));
                common_values.push_back(TO_STR(way.osm_id()));
                common_values.push_back(way.maxspeed_forward_str());
                common_values.push_back(way.maxspeed_backward_str());
                common_values.push_back(way.oneWayType_str());
                common_values.push_back(way.has_attribute("oneway") ? way.get_attribute("oneway") : std::string(""));
                common_values.push_back(TO_STR(config.priority(way.tag_config())));

                auto splits = way.split_me();
                split_count +=  splits.size();
                for (size_t j = 0; j < splits.size(); ++j) {
                    auto length = way.length_str(splits[j]);

                    auto values = common_values;
                    values.push_back(length);
                    values.push_back(splits[j].front()->lon());
                    values.push_back(splits[j].front()->lat());
                    values.push_back(splits[j].back()->lon());
                    values.push_back(splits[j].back()->lat());
                    values.push_back(TO_STR(splits[j].front()->osm_id()));
                    values.push_back(TO_STR(splits[j].back()->osm_id()));
                    values.push_back(way.geometry_str(splits[j]));

                    // cost based on oneway
                    if (way.is_reversed())
                        values.push_back(std::string("-") + length);
                    else
                        values.push_back(length);

                    // reverse_cost
                    if (way.is_oneway())
                        values.push_back(std::string("-") + length);
                    else
                        values.push_back(length);

                    values.push_back(way.name());
                    PQputline(mycon, tab_separated(values).c_str());
                }
            }

            PQputline(mycon, "\\.\n");
            PQendcopy(mycon);

            print_progress(ways.size(), count);
            process_section(ways_columns, Xaction);
            Xaction.exec("DROP TABLE " + temp_table);
            Xaction.commit();
        } catch (const std::exception &e) {
            std::cerr <<  "\n" << e.what() << std::endl;
            std::cerr << "While processing FROM " << start << "th \t to: " << limit << "th way\n";
            std::cerr << "count" << count << " While processing FROM " << start << "th \t to: " << limit << "th way\n";
        }

        start = limit;
    }
}



void Export2DB::process_section(const std::string &ways_columns, pqxx::work &Xaction) const {
    //  std::cout << "Creating indices in temporary table\n";
    auto temp_table(m_tables.ways.temp_name());

    Xaction.exec("CREATE INDEX "+ temp_table + "_gdx ON "+ temp_table + " using gist(the_geom);");
    Xaction.exec("CREATE INDEX ON "+ temp_table + "  USING btree (source_osm)");
    Xaction.exec("CREATE INDEX ON "+ temp_table + "  USING btree (target_osm)");




    //  std::cout << "Deleting  duplicated ways FROM temporary table\n";
    std::string delete_from_temp(
            " DELETE FROM "+ temp_table + " a "
            "     USING " + m_tables.ways.addSchema() + " b "
            "     WHERE a.the_geom ~= b.the_geom AND ST_OrderingEquals(a.the_geom, b.the_geom);");
    Xaction.exec(delete_from_temp);

    //  std::cout << "Updating to existing toplology the temporary table\n";
    fill_source_target(temp_table, m_tables.ways_vertices_pgr.addSchema(), Xaction);

    //  std::cout << "Inserting new vertices in the vertex table\n";
    fill_vertices_table(temp_table, m_tables.ways_vertices_pgr.addSchema(), Xaction);

    //  std::cout << "Updating to new toplology the temporary table\n";
    fill_source_target(temp_table, m_tables.ways_vertices_pgr.addSchema(), Xaction);


    //  std::cout << "Inserting new split ways to '" << addSchema(full_table_name("ways")) << "'\n";
    std::string insert_into_ways(
            " INSERT INTO " + m_tables.ways.addSchema() +
            "(" + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s) "
            " (SELECT " + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s FROM " + temp_table + "); ");
    auto result = Xaction.exec(insert_into_ways);
    std::cout << "\tSplit ways inserted " << result.affected_rows() << "\n";
}





int64_t
Export2DB::get_val(const std::string sql) const {
#if 0
    std::cout << "\nExecuting: \n" << sql << "\n";
#endif
    try {
        pqxx::work Xaction(db_conn);
        auto result = Xaction.exec(sql);
        Xaction.commit();
        if (result.size() == 0) return 0;
        return result[0][0].as<int64_t>();
    } catch (const std::exception &e) {
        std::cout << "\nWARNING: " << e.what() << std::endl;
        std::cout <<  sql << "\n";
    }
    return 0;
}

void
Export2DB::execute(const std::string sql) const {
#if 1
    std::cout << "\nExecuting: \n" << sql << "\n";
#endif
    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec(sql);
        Xaction.commit();
    } catch (const std::exception &e) {
        std::cout << "\nWARNING: " << e.what() << std::endl;
        std::cout <<  sql << "\n";
    }
}



/*
 *
 *  Integrity of the OSM data IS not ensured so failings are ignored
 *
 *  Due to the fact that indexes slow down the process, no index IS created
 *
 *  After all the data IS inserted then its time to create indices & foreign keys
 *
 */
void Export2DB::createFKeys() const {
    std::string sql;
    auto ways =  m_tables.ways.addSchema();
    auto vertices = m_tables.ways_vertices_pgr.addSchema(); 
    auto pois = m_tables.points_of_interest.addSchema();
    auto configuration = m_tables.configuration.addSchema();

    /*
     * configuration:
     */
#if 0
    execute(
            "ALTER TABLE " + configuration
            + "\n  ADD PRIMARY KEY (id);");
#else
    execute(m_tables.configuration.primary_key("id"));
#endif
    execute(
            "ALTER TABLE " + configuration
            + "\n  ADD UNIQUE (tag_id);");

    /*
     * vertices
     */
    execute(
            "ALTER TABLE " + vertices
            + "\n  ADD PRIMARY KEY (id);");
    execute(
            "ALTER TABLE " + vertices
            + "\n  ADD UNIQUE (osm_id);");

    execute(
            " CREATE INDEX ON " + vertices
            + "\n  USING GIST (the_geom);");
    /*
     * Ways
     */
    execute(
            " ALTER TABLE " + ways
            + "\n  ADD PRIMARY KEY (id);");

    execute(
            " ALTER TABLE " + ways
            + "\n  ADD FOREIGN KEY (source) REFERENCES " + vertices + "(id) "
            + "\n  ON UPDATE NO ACTION ON DELETE NO ACTION;");

    execute(
            " ALTER TABLE " + ways
            + "\n  ADD FOREIGN KEY (target) REFERENCES " + vertices + "(id) "
            + "\n  ON UPDATE NO ACTION ON DELETE NO ACTION;");

    execute(
            " ALTER TABLE " + ways
            + "\n  ADD FOREIGN KEY (source_osm) REFERENCES " + vertices + "(osm_id) "
            + "\n  ON UPDATE NO ACTION ON DELETE NO ACTION;");

    execute(
            " ALTER TABLE " + ways
            + "\n  ADD FOREIGN KEY (target_osm) REFERENCES " + vertices + "(osm_id) "
            + "\n  ON UPDATE NO ACTION ON DELETE NO ACTION;");

    execute(
            " ALTER TABLE " + ways
            + "\n  ADD FOREIGN KEY (tag_id) REFERENCES " + configuration + "(tag_id) "
            + "\n  ON UPDATE NO ACTION ON DELETE NO ACTION;");

    execute(
            " CREATE INDEX ON " + ways
            + "\n  USING GIST (the_geom);");

    /*
     * ponitsOfInterest
     */


    execute(
            "\n ALTER TABLE " + pois
            +"\n ADD PRIMARY KEY (pid);");

    execute(
            "\n  UPDATE " + pois + " AS a set (vertex_id, length_m) = (b.id, 0)"
            + "\n  FROM " + vertices + " AS b"
            + "\n  WHERE a.osm_id = b.osm_id;");


    execute(
            "\n  CREATE INDEX ON " + pois
            + "\n  USING GIST (the_geom);");



    execute(
            "\nCREATE OR REPLACE FUNCTION osm2pgrouting_findClosestEdge(radius FLOAT, within FLOAT)"

            "\n RETURNS BIGINT AS"
            "\n $$"
            "\n DECLARE"
            "\n    curr_tot BIGINT;"
            "\n    total BIGINT :=0;"
            "\n    rec RECORD;"
            "\n    factor FLOAT = 0.5;"
            "\n    tooFar BIGINT[];"
            "\n BEGIN"
            "\n    SELECT count(*) FROM " + pois
            +"\n    WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n    INTO rec; "

            +"\n    FOR i IN 1..rec.count LOOP"
            +"\n        curr_tot = osm2pgrouting_updateClosestEdge(radius, within, tooFar);"

            +"          RAISE NOTICE '%: Updated % points of Interest', i, curr_tot;"
            +"\n        --curr_tot := rec.osm2pgrouting_updateClosestEdge;"
            +"\n        total := total + curr_tot;"
            +"\n        IF (curr_tot = 0) THEN"
            +"\n            SELECT pid FROM " + pois
            +"\n                WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n                AND pid not in (SELECT unnest(tooFar))"
            +"\n                limit 1 INTO rec;"
            +"\n            raise notice 'Not within range: pid = %', rec.pid;"
            +"\n            tooFar := tooFar || rec.pid;"
            +"\n            SELECT count(*) FROM (SELECT * FROM " + pois
            +"\n                WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n                AND pid not in (SELECT unnest(tooFar)) LIMIT 1) a  INTO rec;"

            +"\n            EXIT WHEN rec.count = 0;"
            +"\n        END IF;"
            +"\n    END LOOP;"

            +"\n    return total;"
            +"\n END;"
            +"\n $$"
            +"\n LANGUAGE plpgsql;"
            );

    execute(
            "CREATE OR REPLACE FUNCTION osm2pgrouting_updateClosestEdge(radius FLOAT, within FLOAT, tooFar BIGINT[])"
            "\n RETURNS BIGINT AS"
            "\n $$"
            "\n DECLARE"
            "\n    curr_tot BIGINT;"
            "\n BEGIN"
            "\n        WITH "
            "\n        poi AS ("
            "\n            SELECT ST_buffer(the_geom::geography, $1)::geometry AS bufferPois,"
            "\n            ST_buffer(the_geom::geography, $1 + $2)::geometry AS bufferWays"
            "\n            FROM " + pois
            +"\n            WHERE vertex_id IS NULL AND edge_id IS NULL"
            +"\n            AND pid not in (SELECT unnest(tooFar))"
            +"\n            limit 1"
            +"\n        ),"
            +"\n        pois AS ("
            +"\n            SELECT * FROM " + pois + ", poi"
            +"\n            WHERE ST_Within(the_geom, bufferPois) "
            +"\n            AND vertex_id IS NULL AND edge_id IS NULL"
            +"\n            AND pid not in (SELECT unnest(tooFar))"
            +"\n        ),"
            +"\n        wayss AS ("
            +"\n            SELECT * FROM " + ways + ", poi"
            +"\n            WHERE ST_Intersects(the_geom, bufferWays)"
            +"\n        ),"
            +"\n        first AS ("
            +"\n            SELECT   ways.id AS wid,"
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
            +"\n            SELECT third.*, b.id  FROM third join   ways_vertices_pgr AS b ON (third.v_osm_id = b.osm_id)"
            +"\n            UNION"
            +"\n            SELECT first.vid, first.wid, first.fraction, first.dist, NULL AS v_osm_id, NULL::bigint AS id FROM first, second WHERE dist = min AND fraction not in (0, 1)"
            +"\n        )"

            +"\n        UPDATE   pointsofinterest AS pois   SET (vertex_id, edge_id, fraction, length_m) = (last.id, last.wid, last.fraction, last.dist)"
            +"\n        FROM last WHERE  pois.osm_id = last.vid;"
            +"\n        GET DIAGNOSTICS curr_tot = ROW_COUNT;"
            +"\n    return curr_tot;"
            +"\n END;"
            +"\n $$"
            +"\n LANGUAGE plpgsql;"
            );

    std::string array;
    int64_t total = 0;
    auto limit = get_val(
            "SELECT count(*) FROM " + pois
            + "\n  WHERE vertex_id IS NULL AND edge_id IS NULL");

#if 0
    std::cout << "\nlimit " << limit;
#endif

    std::cout << "\nFinding closest edge to " << limit << " Points Of Interest\n";
    for (int64_t i = 0; i < limit; ++i) {
        auto curr_tot = get_val(
                "SELECT osm2pgrouting_updateClosestEdge(200, 50, ARRAY[" + array + "]::BIGINT[])");
        total += curr_tot;

        if (curr_tot == 0) {
            auto pid_outOfRange = get_val(" SELECT pid FROM " + pois
                    +"\n  WHERE vertex_id IS NULL AND edge_id IS NULL"
                    +"\n  AND pid not in (SELECT unnest(ARRAY[" + array +"]::BIGINT[]))"
                    +"\n  limit 1;");
            if (array.empty()) {
                array += boost::lexical_cast<std::string>(pid_outOfRange);
            } else {
                array += "," + boost::lexical_cast<std::string>(pid_outOfRange);
            }
#if 0
            std::cout << "\nNot within distance: pid = " << pid_outOfRange;
#endif

            if (get_val(
                    +"SELECT count(*) FROM (SELECT * FROM " + pois
                    +"\n  WHERE vertex_id IS NULL AND edge_id IS NULL"
                    +"\n  AND pid not in (SELECT unnest(ARRAY[" + array +"]::BIGINT[]))) AS a"
                    ) == 0) break;
        }
#if 0
        std::cout << i << ": total " << total;
#endif
        print_progress(limit, total);
    }

    if (!array.empty()) {
        std::cout << "\nNo edge found within distance (200 + 50)mts on pid(s): " << array << "\n";
    }


    execute(
            "\n WITH "
            "\n base AS ("
            "\n     SELECT pid, w.id AS wid, w.the_geom AS wgeom, p.the_geom AS pgeom"
            "\n     FROM " + pois + " AS p JOIN " + ways + " AS w ON (edge_id = w.id)"
            + "\n     WHERE edge_id IS not NULL"
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

            + "\n UPDATE " + pois + " set side = case when val>0 then 'L' when val<0 then 'R' else 'B' end "
            + "\n FROM last "
            + "\n WHERE last.pid = " + pois + ".pid;"

            );
}

}  // namespace osm2pgr
