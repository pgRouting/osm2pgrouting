/*osm2pgRouting

Copyright (c) 2015 Sarthak Agarwal
sarthak0415@gmail.com

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

*/

/*to compile and run
>>  g++ prog_options.cpp -o a -lboost_program_options -g -O3 -std=c++0x -Wall -pedantic
>> ./a --help
*/

#include <boost/program_options.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
namespace po = boost::program_options;

using namespace std;


void get_option_description(po::options_description &od_desc) {
    /* po::options_description help_od_desc("Help"),
        required_od_desc("Required options"),
        optional_od_desc("Optional options");
    */
    po::options_description help_od_desc("Help"),
        general_od_desc("General"),
        db_options_od_desc("Database options"),
        not_used_od_desc("Not used currently");

    help_od_desc.add_options()
        // help
        ("help", "Produce help message for this version.")
        ("version,v", "Print version string")
        ;

    general_od_desc.add_options()
        // general
        ("file,f", po::value<string>()->required(), "REQUIRED: Name of the osm file.")
        ("conf,c", po::value<string>()->default_value("/usr/share/osm2pgrouting/mapconfig.xml"), "Name of the configuration xml file.")
        //("conf,c", po::value<string>()->required()->default_value("/usr/share/osm2pgrouting/mapconfig.xml"), "Name of the configuration xml file.")
        ("schema", po::value<string>()->default_value(""), "Database schema to put tables.\n  blank:\t defaults to default schema dictated by PostgreSQL search_path.")
        ("prefix", po::value<string>()->default_value(""), "Prefix added at the beginning of the table names.")
        ("suffix", po::value<string>()->default_value(""), "Suffix added at the end of the table names.")
        ("addnodes", "Import the osm_nodes table.")
        ("clean", "Drop previously created tables.")
        ;

    db_options_od_desc.add_options()
        // database options
        ("dbname,d", po::value<string>()->required(), "Name of your database (Required).")
        ("username,U", po::value<string>()->default_value("postgres"), "Name of the user, which have write access to the database.")
        ("host,h", po::value<string>()->default_value("localhost"), "Host of your postgresql database.")
        ("port,p", po::value<string>()->default_value("5432"), "db_port of your database.")
        ("password,W", po::value<string>()->default_value(""), "Password for database access.")
        ;

    not_used_od_desc.add_options()
        ("threads,t", po::value<bool>()->default_value(false), "threads.")
        ("multimodal,m", po::value<bool>()->default_value(false), "multimodal.")
        ("multilevel,l", po::value<bool>()->default_value(false), "multilevel.")
        ;

    od_desc.add(help_od_desc).add(general_od_desc).add(db_options_od_desc);  // .add(not_used_od_desc);

    return;
}



void
process_command_line( po::variables_map &vm) {
    std::cout << "***************************************************\n";
    std::cout << "           COMMAND LINE CONFIGURATION             *\n";
    std::cout << "***************************************************\n";
    std::cout << "Filename = " << vm["file"].as<string>() << "\n";
    std::cout << "Configuration file = " << vm["conf"].as<string>() << "\n";
    std::cout << "host = " << vm["host"].as<std::string>() << "\n";
    std::cout << "port = " << vm["port"].as<std::string>() << "\n";
    std::cout << "dbname = " << vm["dbname"].as<std::string>() << "\n";
    std::cout << "username = " << vm["username"].as<std::string>() << "\n";
    std::cout << "password = " << vm["password"].as<string>() << "\n";
    std::cout << "schema= " << vm["schema"].as<string>() << "\n";
    std::cout << "prefix = " << vm["prefix"].as<string>() << "\n";
    std::cout << "suffix = " << vm["suffix"].as<string>() << "\n";
    std::cout << (vm.count("clean")? "D" : "Don't d") << "rop tables\n";
    std::cout << (vm.count("addnodes")? "A" : "Don't a") << "dd nodes\n";
    std::cout << "***************************************************\n";
}
