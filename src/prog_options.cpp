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

#if 0
void get_option_description(po::options_description &od_desc,
    po::options_description &cmdline_options,
    po::options_description &config_file_options,
    string &config_file){

    string filename ;
    po::options_description generic("Generic options");
    generic.add_options()
        ("file,f",po::value<string>(&filename),"filename")
        ("conf,c",po::value<string>(&config_file)->default_value("prog_options.cfg"),"configuration file path/name (ask vicky)")
        ("help", "produce help message")
        ;

    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
    string dbname,username,host,port,password;
    po::options_description config("Configuration");
    config.add_options()
        ("dbname,d",po::value<string>(&dbname),"database name to be imported")
        ("username,u",po::value<string>(&username),"name of the username of the database")
        ("host,h",po::value<string>(&host),"host name, eg: localhost(127.0.0.1)")
        ("port,p",po::value<string>(&port),"port name, eg: 8080")
        ("password,passwd",po::value<string>(&password),"password")
        ;

    //Dont need it as of now
    /* 
    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value< vector<string> >(), "input file")
        ;
    */
    
    cmdline_options.add(generic).add(config);

    config_file_options.add(config);

    od_desc.add(generic).add(config);

    return ;
}
#endif
void get_option_description(po::options_description &od_desc,
 string &file,
 string &cFile,
 string &host,
 string &user,
 string &db_port,
 string &dbname,
 string &passwd,
 string &prefixtables,
 bool &skipnodes,
 bool &clean){

    
    od_desc.add_options()
        //help
        ("help", "produce help message")
        //required
        ("file,f",po::value<string>(&file)->required(),"name of your osm xml file")
        ("cFile,c",po::value<string>(&cFile)->required(),"name of your configuration xml file")
        ("dbname,d",po::value<string>(&dbname)->required(),"name of your database")
        ("user,u",po::value<string>(&user)->required(),"name of the user, which have write access to the database")
        //optional
        ("host,h",po::value<string>(&host)->default_value("127.0.0.1"),"host of your postgresql database (default: 127.0.0.1)")
        ("db_port,p",po::value<string>(&db_port)->default_value("5432"),"db_port of your database (default: 5432)")
        ("passwd",po::value<string>(&passwd),"password for database access")
        ("prefixtables,t",po::value<string>(&prefixtables)->default_value("")," add at the beginning of table names")
        //bool
        ("clean",po::value<bool>(&clean)->default_value(false),"drop previously created tables")
        ("skipnodes,s",po::value<bool>(&skipnodes)->default_value(false),"don't import the nodes table")
        ;
    return ;
}

int process_command_line(
  po::variables_map &vm, 
  po::options_description &od_desc) {

    if (vm.count("help")) {
        cout << od_desc << "\n";
        return 0;
    }

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

    if (vm.count("user")) 
        std::cout << "user = " << vm["user"].as<std::string>() << "\n";
    else
        std::cout << "Parameter: user missing\n";

    if (vm.count("host")) 
        std::cout << "host = " << vm["host"].as<std::string>() << "\n";
    else
        std::cout << "Parameter: host missing\n";

    if (vm.count("db_port")) 
        std::cout << "db_port = " << vm["db_port"].as<std::string>() << "\n";
    else
        std::cout << "Parameter: db_port missing\n";

    if (vm.count("passwd"))
        cout << "passwd is: " << vm["passwd"].as<string>() << "\n";
    else
        std::cout << "Parameter: passwd missing\n";  

    if (vm.count("prefixtables"))
        cout << "prefixtables is: " << vm["prefixtables"].as<string>() << "\n";
    else
        std::cout << "Parameter: prefixtables missing\n";

    if (vm.count("clean"))
        cout << "clean is: " << vm["clean"].as<bool>() << "\n";
    else
        std::cout << "Parameter: clean missing\n";

    if (vm.count("skipnodes"))
        cout << "skipnodes is: " << vm["skipnodes"].as<bool>() << "\n";
    else
        std::cout << "Parameter: skipnodes missing\n";         
    

    if (vm.count("dbname") & vm.count("username") & vm.count("host") & vm.count("password") ) {
        std::cout << "Parameters: \n"
             << vm["dbname"].as<std::string>() << "\n"
             << vm["username"].as<std::string>() << "\n"
             << vm["host"].as<std::string>() << "\n"
             << vm["password"].as<std::string>() << ".\n";

        return 2;
    } else {
        std::cout << "Missing parameter.\n";
        std::cout << od_desc << "\n";
        return 1;
    }

}

#if 0
int main(int ac, char* av[])
{
    try {
        
    //-->Code for program option begin

        //Declare a group of options that will be 
        // allowed only on command line

        //cmdline_options have options that can be given on command line
        //config_file_options have options that can be given through config file only
        //od_desc has all the commands

        po::options_description od_desc("Allowed options");
        
        #if 0
        po::options_description cmdline_options;
        po::options_description config_file_options;
        string config_file; 
        get_option_description(od_desc,cmdline_options,config_file_options,config_file);
        #endif
        get_option_description(od_desc);
        
        
        
        //Do we need a positional option for filename/dbname??
        /*
        po::positional_options_description p;
        p.add("input-file", -1);
        */

        po::variables_map vm;

        #if 0
        store(po::command_line_parser(ac, av).options(cmdline_options).run(), vm);
        #endif
        store(po::command_line_parser(ac, av).options(od_desc).run(), vm);
        notify(vm);
        
        #if 0
        ifstream ifs(config_file.c_str());
        if (!ifs)
        {
            cout << "can not open config file: " << config_file << "\n";
            return 0;
        }
        else
        {
            cout << "can open config file: " << config_file << "\n";
            store(parse_config_file(ifs, config_file_options), vm);
            //notify(vm);
        }
        
        process_comand_line(vm,od_desc,cmdline_options,config_file_options);
        #endif
        process_command_line(vm,od_desc);

    //Code for program option end <--



        
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }
    return 0;
}
#endif