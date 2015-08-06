/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt               					   *
 *   gentoo.murray@gmail.com   											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <unistd.h>
#include <string>
#include <ctime>
#include "./stdafx.h"
#include "./Configuration.h"
#include "./ConfigurationParserCallback.h"
#include "./OSMDocument.h"
#include "./OSMDocumentParserCallback.h"
#include "./Way.h"
#include "./Node.h"
#include "./Relation.h"
#include "./Export2DB.h"
#include "./prog_options.h"

// using namespace osm;
// using namespace xml;
// using namespace std;


int main(int argc, char* argv[]) {
    //  Start Timers
    clock_t begin = clock();
    try {
        // ..prog_options code begin..

        // std::string file,cFile,host,user,db_port,dbname,passwd,prefixtables,suffixtables;
        // bool skipnodes,clean,threads,multimodal,multilevel;

        po::options_description od_desc("Allowed options");
        get_option_description(od_desc);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
            options(od_desc).run(), vm);

        if (vm.count("help")) {
            std::cout << od_desc << "\n";
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

        process_command_line(vm, od_desc);

        auto file(vm["file"].as<string>());
        auto cFile(vm["conf"].as<string>());
        auto host(vm["host"].as<std::string>());
        auto user(vm["user"].as<std::string>());
        auto db_port(vm["db_port"].as<std::string>());
        auto dbname(vm["dbname"].as<std::string>());
        auto passwd(vm["passwd"].as<std::string>());
        auto prefixtables(vm["prefix"].as<std::string>());
        auto suffixtables(vm["suffix"].as<std::string>());
        auto skipnodes(vm["skipnodes"].as<bool>());
        auto clean(vm["clean"].as<bool>());

        /* variable to be used later 
        auto threads (vm["threads"].as<bool>() );
        auto multimodal (vm["multimodal"].as<bool>() );
        auto multilevel (vm["multilevel"].as<bool>() );
        */

        // !!prog_options code end!!

        Export2DB dbConnection(vm);
        if (dbConnection.connect() == 1)
            return 1;

        xml::XMLParser parser;

        std::cout << "Trying to load config file " << cFile.c_str() << endl;

        Configuration* config = new Configuration();
        ConfigurationParserCallback cCallback(*config);

        std::cout << "Trying to parse config" << endl;

        int ret = parser.Parse(cCallback, cFile.c_str());
        if (ret != 0) {
            cerr << "Failed to parse config file " << cFile.c_str() << endl;
            return 1;
        }

        std::cout << "Trying to load data" << endl;

        OSMDocument* document = new OSMDocument(*config);
        OSMDocumentParserCallback callback(*document);

        std::cout << "Trying to parse data" << endl;

        ret = parser.Parse(callback, file.c_str());
        if (ret != 0) {
            if (ret == 1)
                cerr << "Failed to open data file" << endl;
            cerr << "Failed to parse data file " << file.c_str() << endl;
            return 1;
        }

        std::cout << "Split ways" << endl;

        document->SplitWays();
        //############# Export2DB
        {
           if (clean) {
            std::cout << "Dropping tables..." << endl;

            dbConnection.dropTables();
        }

        std::cout << "Creating tables..." << endl;
        dbConnection.createTables();

        std::cout << "Adding tag types and classes to database..." << endl;
        dbConnection.exportTypesWithClasses(config->m_Types);

            std::cout << "Adding relations to database..." << endl;
            dbConnection.exportRelations(document->m_Relations, config);

            // Optional user argument skipnodes will not
            // add nodes to the database
            // (saving a lot of time if not necessary)
            if (!skipnodes) {
                std::cout << "Adding nodes to database..." << endl;
                dbConnection.exportNodes(document->m_Nodes);
            }

            std::cout << "Adding ways to database..." << endl;
            dbConnection.exportWays(document->m_SplittedWays, config);

            // TODO: make some free memory, document will be not used anymore,
            // so there will be more memory available to future DB operations.

            std::cout << "Creating topology..." << endl;
            dbConnection.createTopology();
        }

        //#############

        /*
        std::vector<Way*>& ways= document.m_Ways;
        std::vector<Way*>::iterator it(ways.begin());
        std::vector<Way*>::iterator last(ways.end());
        while(it!=last)
        {
            Way* pWay = *it;
    
            if (!pWay->name.empty())
            {
                if (pWay->m_NodeRefs.empty())
                {
                    std::std::cout << pWay->name.c_str() << endl;
                }
                else
                {
                    Node* n0 = pWay->m_NodeRefs.front();
                    Node* n1 = pWay->m_NodeRefs.back();
                    //if (n1->numsOfUse==1)
                    //std::cout << "way-id: " << pWay->id << " name: " << pWay->name <<endl;
                    //std::std::cout << n0->lon << " "  << n0->lat << " " << n1->lon << " " << n1->lat << " " << pWay->name.c_str() << " highway: " << pWay->highway.c_str() << " Start numberOfUse: " << n0->numsOfUse << " End numberOfUse: " << n1->numsOfUse  << " ID: " << n1->id <<  endl;
                }
            }
            if (pWay->id == 20215432) // Pfaenderweg
            {
                std::cout << pWay->name << endl;
                int a=4;
            }
            ++it;
        }
        */

        std::cout << "#########################" << endl;

        std::cout << "size of streets: " << document->m_Ways.size() <<    endl;
        std::cout << "size of splitted ways : " << document->m_SplittedWays.size() <<    endl;

        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

        std::cout << "Total processing time: -> " << elapsed_secs << " seconds\n";
        std::cout << "#########################" << endl;
        //  string n;
        //  getline(cin, n);
        return 0;
    }
    catch (exception &e) {
        std::cout << e.what() << endl;
        return 1;
    }
}
