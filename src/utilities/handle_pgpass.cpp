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


#include <iostream>
#include <boost/program_options.hpp>
#include <sys/stat.h>
#include <fstream>
#include <cstdlib>
#include <string>
#include <unistd.h>

namespace po = boost::program_options;

void
handle_pgpass(po::variables_map &vm) {

    if (!vm["password"].as<std::string>().empty()) {
        /*
         * nothing to do password is given
         */
        return;
    }



    std::string file;
    auto filename(getenv("PGPASSFILE"));
    if (!filename) {
#if 0
        std::cout << "No PGPASSFILE found \n";
        std::cout << "Looking for .pgpass \n";
#endif
        auto homedir(getenv("HOME"));
        if (!homedir) {
            std::cout << "No $HOME found \n";
            return;
        }
#if 0
        std::cout << "home directory" << homedir << "\n";
#endif
        file = std::string(homedir) + "/.pgpass";
    }
    else {
        file = filename;
    }
    std::ifstream infile(file.c_str());
    if (!infile.good()) {
        return;
    }


    std::string host;
    std::string port;
    std::string dbase;
    std::string user;
    std::string passwd;
    std::string username = vm["username"].as<std::string>().empty() ?
        getenv("USER") : vm["username"].as<std::string>();
    vm.at("username").value() = username;
    while (std::getline(infile, host, ':')) {
        std::getline(infile, port, ':');
        std::getline(infile, dbase, ':');
        std::getline(infile, user, ':');
        std::getline(infile, passwd);
        if ((host == ""  || host == "*" || host == vm["host"].as<std::string>())
                && (port == "*" || port == vm["port"].as<std::string>())
                && (dbase == "*" || dbase == vm["dbname"].as<std::string>())
                && (user == "*" || user == username)
                && (dbase == "*" || host == vm["dbname"].as<std::string>())) {
            infile.close();
#if 0
            std::cout << passwd << "\n";
#endif
            vm.at("password").value() = passwd;
            return;
        }
    }
    return;
}
