/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#if __GNUC__ > 5 || \
    (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define WITH_TIME
#endif

#include <unistd.h>
#include <string>
#include <iostream>

#ifdef WITH_TIME
#include <ctime>
#include <chrono>
#endif

#include <pqxx/pqxx>

#include "parser/ConfigurationParserCallback.h"
#include "parser/OSMDocumentParserCallback.h"
#include "osm_elements/OSMDocument.h"
#include "database/Export2DB.h"
#include "utilities/handle_pgpass.h"
#include "utilities/prog_options.h"

#if defined(__linux__)
static
size_t lines_in_file(const std::string file_name) {
    FILE *in;
    char buff[512];
    std::string command = "wc -l  " + file_name;

    if (!(in = popen(command.c_str(), "r"))) {
        std::cerr << "File not found" << file_name << "\n";
        exit(1);
    }

    std::string word;
    if (fgets(buff, 512, in) != NULL) {
        word = buff;
    }
    pclose(in);
    std::istringstream iss(word);
    std::string number;
    iss >> number;
    try {
        return boost::lexical_cast<size_t>(number);
    } catch (...) {
        std::cerr << "File not found" << file_name << "\n";
        exit(1);
    }
}
#endif


int main(int argc, char* argv[]) {
#ifdef WITH_TIME
    /*
     *   Start Timers
     *      version prior to 4.7.0 std::chrono::steady_clock was not implemented.
     *      so using timers work on g++4.7 up
     */
    clock_t begin = clock();
    std::time_t start_t = std::time(NULL);
    std::chrono::steady_clock::time_point begin_elapsed =
        std::chrono::steady_clock::now();
#endif

    try {
        po::options_description od_desc("Allowed options");
        get_option_description(od_desc);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                options(od_desc).run(), vm);

        if (vm.count("help")) {
            std::cout << od_desc << "\n";
            return 0;
        }

        if (vm.count("version")) {
            std::cout << "This is osm2pgrouting Version 2.3.7\n";
            return 0;
        }

        try {
            notify(vm);
        }

        catch(exception &ex) {
            std::cout << ex.what() << "\n";
            std::cout << od_desc << "\n";
            return 0;
        }

#ifdef WITH_TIME
        std::cout << "Execution starts at: " << std::ctime(&start_t) << "\n";
#endif
        process_command_line(vm);

        auto dataFile(vm["file"].as<string>());
        auto confFile(vm["conf"].as<string>());
        auto clean(vm.count("clean"));
        auto no_index(vm.count("no-index"));

        handle_pgpass(vm);
        std::string connection_str(
                    "host=" + vm["host"].as<std::string>()
                    + " user=" +  vm["username"].as<std::string>()
                    + " dbname=" + vm["dbname"].as<std::string>()
                    + " port=" + vm["port"].as<std::string>()
                    + " password=" + vm["password"].as<std::string>());
        try {
            cout << "Testing database connection: "
                << vm["dbname"].as<std::string>()
                << endl;
            pqxx::connection C(connection_str);
            if (C.is_open()) {
                cout << "database connection successful: " << C.dbname() << endl;
            } else {
                cout << "Can't open database" << endl;
                return 1;
            }
            C.disconnect ();
        }catch (const std::exception &e){
            cerr << e.what() << std::endl;
            return 1;
        }

        /*
         * preparing the databasse
         */
        std::cout << "Connecting to the database"  << endl;
        osm2pgr::Export2DB dbConnection(vm, connection_str);
        if (dbConnection.connect() == 1)
            return 1;

#ifndef NDEBUG
        dbConnection.install_postGIS();
#endif

        if (!dbConnection.has_extension("postgis")) {
            std::cout << "ERROR: postGIS not found\n";
            std::cout << "   HINT: CREATE EXTENSION postGIS\n";
            return 1;
        }
        if ((vm.count("attributes") || vm.count("tags") || vm.count("addnodes"))
                && !dbConnection.has_extension("hstore")) {
            std::cout << "ERROR: hstore not found\n";
            std::cout << "   HINT: CREATE EXTENSION hstore\n";
            return 1;
        }

        if (clean) {
            std::cout << "\nDropping tables..." << endl;
            dbConnection.dropTables();
        }
        std::cout << "\nCreating tables..." << endl;
        dbConnection.createTables();

        /*
         * End: preparing the databasse
         */

        std::cout << "Opening configuration file: " << confFile.c_str() << endl;
        osm2pgr::Configuration config;
        osm2pgr::ConfigurationParserCallback cCallback(config);


        std::cout << "    Parsing configuration\n" << endl;
        xml::XMLParser parser;
        int ret = parser.Parse(cCallback, confFile.c_str());
        if (ret != 0) {
            cout << "Failed to open / parse config file\n"
                << confFile.c_str()
                << endl;
            return 1;
        }
        std::cout << "Exporting configuration ...\n";
        dbConnection.export_configuration(config.types());
        std::cout << "  - Done \n";


#if defined(__linux__)
        std::cout << "Counting lines ...\n";
        auto total_lines = lines_in_file(dataFile);
        std::cout << "  - Done \n";

        std::cout << "Opening data file: "
            << dataFile
            << "\ttotal lines: "
            << total_lines
            << endl;
#else
        size_t total_lines = 0;
#endif
        osm2pgr::OSMDocument document(config, vm, dbConnection, total_lines);
        osm2pgr::OSMDocumentParserCallback callback(document);

        std::cout << "    Parsing data\n" << endl;
        ret = parser.Parse(callback, dataFile.c_str());
        if (ret != 0) {
            cerr << "Failed to open / parse data file " << dataFile << endl;
            return 1;
        }
        std::cout << "    Finish Parsing data\n" << endl;
        if (document.nodeErrs()) {
            std::cerr << "******\nNOTICE:  Found " << document.nodeErrs() << " node references with no <node ... >\n*****";
        }

        //############# Export2DB
        {

            std::cout << "Adding auxiliary tables to database..." << endl;


            std::cout << "\nExport Ways ..." << endl;
            dbConnection.exportWays(document.ways(), config);

            if (!no_index) {
                std::cout << "\nCreating indexes ..." << endl;
                dbConnection.createFKeys();
            }

            std::cout << "\nProcessing Points of Interest ..." << endl;
            dbConnection.process_pois();

        }


        std::cout << "#########################" << endl;

        std::cout << "size of streets: " << document.ways().size() << endl;

#ifdef WITH_TIME
        clock_t end = clock();
        double elapsed_secs =
            static_cast<double>(end - begin)
            / static_cast<double>(CLOCKS_PER_SEC);

        std::time_t end_t = std::time(NULL);
        std::chrono::steady_clock::time_point end_elapsed =
            std::chrono::steady_clock::now();

        typedef std::chrono::duration<int, std::milli> millisecs_t;
        millisecs_t duration = std::chrono::duration_cast<millisecs_t>(
                end_elapsed - begin_elapsed);

        std::cout << "Execution started at: " << std::ctime(&start_t);
        std::cout << "Execution ended at:   " << std::ctime(&end_t);
        std::cout << "Elapsed time: "
            << static_cast<double>(duration.count())/static_cast<double>(1000)
            << " Seconds.\n";
        std::cout << "User CPU time: -> " << elapsed_secs << " seconds\n";
#endif

        std::cout << "#########################" << endl;

        exit(0);
    }
    catch (exception &e) {
        std::cout << e.what() << endl;
        return 1;
    }
    catch (string &e) {
        std::cout << e << endl;
        return 1;
    }
    catch (...) {
        std::cout << "Terminating" << endl;
        return 1;
    }
}
