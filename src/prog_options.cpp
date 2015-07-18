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

void get_option_description(po::options_description &od_desc){

    string file,cFile,host,user,db_port,dbname,passwd,prefixtables;
    bool skipnodes,clean;
    po::options_description help_od_desc("Help"),required_od_desc("Required options"),optional_od_desc("Optional options");
    

    help_od_desc.add_options()
        //help
        ("help", "produce help message for this version")
        ;

    required_od_desc.add_options()
        //required
        ("file,f",po::value<string>()->required(),"Name of your osm xml file")
        ("cFile,c",po::value<string>()->required(),"Name of your configuration xml file")
        ("dbname,d",po::value<string>()->required(),"Name of your database")
        ;
    
    optional_od_desc.add_options()
        //optional
        ("user,u",po::value<string>()->default_value(getlogin()),"Name of the user, which have write access to the database, (default : username).")
        ("host,h",po::value<string>()->default_value("127.0.0.1"),"Host of your postgresql database (default: 127.0.0.1).")
        ("db_port,p",po::value<string>()->default_value("5432"),"db_port of your database (default: 5432).")
        ("passwd",po::value<string>()->default_value(""),"Password for database access (default: \"\").")
        ("prefixtables,t",po::value<string>()->default_value(""),"Add at the beginning of table names (default: \"\").")
        //bool
        ("clean",po::value<bool>(&clean)->default_value(false),"Drop previously created tables (default: false).")
        ("skipnodes,s",po::value<bool>(&skipnodes)->default_value(false),"Don't import the node table (default: false).")
        ;

    od_desc.add(help_od_desc).add(required_od_desc).add(optional_od_desc);
        
    return ;
}

int process_command_line(
  po::variables_map &vm, 
  po::options_description &od_desc) {

    if (vm.count("file"))
        cout << "Filename is: " << vm["file"].as<string>() << "\n";
    else
        std::cout << "Parameter: file missing\n";

    if (vm.count("cFile"))
        cout << "Configuration XML Filename is: " << vm["cFile"].as<string>() << "\n";
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
    std::cout << "prefixtables is: " << vm["prefixtables"].as<string>() << "\n";
    std::cout << "clean is: " << vm["clean"].as<bool>() << "\n";
    std::cout << "skipnodes is: " << vm["skipnodes"].as<bool>() << "\n";

    if (vm.count("dbname") & vm.count("user") & vm.count("host") & vm.count("passwd") ) {
        return 2;
    } else {
        std::cout << "Missing Database connectivity parameter.\n";
        return 1;
    }

}