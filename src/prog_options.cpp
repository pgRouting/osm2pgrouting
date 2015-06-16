/*PGR

Copyright (c) 2015 Celia Virginia Vergara Castillo
vicky_vergara@hotmail.com

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


#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " ")); 
    return os;
}


int main(int ac, char* av[])
{
    try {
        
        //Declare a group of options that will be 
        // allowed only on command line
        string filename, config_file;
        po::options_description generic("Generic options");
        generic.add_options()
            ("file,f",po::value<string>(&filename),"filename")
            ("conf,c",po::value<string>(&config_file)->default_value("osm2pgrouting.cfg"),"configuration file path/name (ask vicky)")
            ("help", "produce help message")
            ;
    
        // Declare a group of options that will be 
        // allowed both on command line and in
        // config file
        string dbname,user,host,port,password;
        po::options_description config("Configuration");
        config.add_options()
            ("dbname,d",po::value<string>(&dbname),"database name to be imported")
            ("user,u",po::value<string>(&user),"name of the user of the database")
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
        
        po::options_description cmdline_options;
        cmdline_options.add(generic).add(config);

        po::options_description config_file_options;
        config_file_options.add(config);

        po::options_description visible("Allowed options");
        visible.add(generic).add(config);
        
        //Do we need a positional option for filename/dbname??
        /*
        po::positional_options_description p;
        p.add("input-file", -1);
        */

        po::variables_map vm;
        store(po::command_line_parser(ac, av).
              options(cmdline_options).run(), vm);
        notify(vm);
        
        ifstream ifs(config_file.c_str());
        if (!ifs)
        {
            cout << "can not open config file: " << config_file << "\n";
            return 0;
        }
        else
        {
            store(parse_config_file(ifs, config_file_options), vm);
            notify(vm);
        }
    
        if (vm.count("help")) {
            cout << visible << "\n";
            return 0;
        }

        if (vm.count("file"))
        {
            cout << "Filename is: " 
                 << vm["file"].as<string>() << "\n";
        }
        
        if (vm.count("conf"))
        {
            cout << "configuration filename is: " 
                 << vm["conf"].as<string>() << "\n";
        }
        
        if (vm.count("dbname"))
        {
            cout << "Database name is: " 
                 << vm["file"].as<string>() << "\n";
        }

        if (vm.count("user"))
        {
            cout << "Userame is: " 
                 << vm["user"].as<string>() << "\n";
        }
        
        if (vm.count("host"))
        {
            cout << "Hostame is: " 
                 << vm["host"].as<string>() << "\n";
        }

        if (vm.count("port"))
        {
            cout << "portname is: " 
                 << vm["port"].as<string>() << "\n";
        }

        if (vm.count("password"))
        {
            cout << "password is: " 
                 << vm["password"].as<string>() << "\n";
        }
        
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}
