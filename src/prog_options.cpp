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


#include <boost/config.hpp>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;


void get_option_description(po::options_description &od_desc) {
    // po::options_description help_od_desc("Help"),required_od_desc("Required options"),optional_od_desc("Optional options");
    po::options_description help_od_desc("Help"), general_od_desc("General"), db_options_od_desc("Database options"), not_used_od_desc("Not used currently");

    help_od_desc.add_options()
        // help
        ("help", "Produce help message for this version.")
        ;

    general_od_desc.add_options()
        // general
        ("file,f", po::value<string>()->required(), "Name of your osm xml file (Required).")
        ("prefix", po::value<string>()->default_value("pgr_"), "Add at the beginning of table names.")
        ("suffix", po::value<string>()->default_value("_car"), "Add at the end of table names.")
        ("skipnodes,s", po::value<bool>()->default_value(false), "Don't import the node table.")
        ("conf,c", po::value<string>()->required()->default_value("/usr/share/osm2pgrouting/mapconfig.xml"), "Name of your configuration xml file.")
        ;

    db_options_od_desc.add_options()
        // database options
        ("dbname,d", po::value<string>()->required(), "Name of your database (Required).")
        ("user,u", po::value<string>()->default_value(getlogin()), "Name of the user, which have write access to the database.")
        ("host,h", po::value<string>()->default_value("localhost"), "Host of your postgresql database.")
        ("db_port,p", po::value<string>()->default_value("5432"), "db_port of your database.")
        ("passwd", po::value<string>()->default_value(""), "Password for database access.")
        ("clean", po::value<bool>()->default_value(false), "Drop previously created tables.")
        ;

    not_used_od_desc.add_options()
        ("threads,t", po::value<bool>()->default_value(false), "threads.")
        ("multimodal,m", po::value<bool>()->default_value(false), "multimodal.")
        ("multilevel,l", po::value<bool>()->default_value(false), "multilevel.")
        ;

    od_desc.add(help_od_desc).add(general_od_desc).add(db_options_od_desc).add(not_used_od_desc);

    return;
}

int process_command_line(
  po::variables_map &vm,
  po::options_description &od_desc) {
    if (vm.count("file"))
        cout << "Filename is: " << vm["file"].as<string>() << "\n";
    else
        std::cout << "Parameter: file missing\n";

    if (vm.count("conf"))
        cout << "Configuration XML Filename is: " << vm["conf"].as<string>() << "\n";
    else
        std::cout << "Parameter: config file name missing\n";

    if (vm.count("dbname"))
        std::cout << "dbname = " << vm["dbname"].as<std::string>() << "\n";
    else
        std::cout << "Parameter: dbname missing\n";

    std::cout << "user = " << vm["user"].as<std::string>() << "\n";
    std::cout << "host = " << vm["host"].as<std::string>() << "\n";
    std::cout << "db_port = " << vm["db_port"].as<std::string>() << "\n";
    std::cout << "passwd is: " << vm["passwd"].as<string>() << "\n";
    std::cout << "prefix is: " << vm["prefix"].as<string>() << "\n";
    std::cout << "suffix is: " << vm["suffix"].as<string>() << "\n";
    std::cout << "clean is: " << vm["clean"].as<bool>() << "\n";
    std::cout << "skipnodes is: " << vm["skipnodes"].as<bool>() << "\n";

    if ( vm.count("dbname") & vm.count("user") & vm.count("host") & vm.count("passwd") ) {
        return 2;
    } else {
        std::cout << "Missing Database connectivity parameter.\n";
        return 1;
    }

    return 0;
}
