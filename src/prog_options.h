#ifndef _prog_options_h
#define _prog_options_h

#include <boost/config.hpp>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

void get_option_description(po::options_description &od_desc);
int process_command_line(po::variables_map &vm,po::options_description &od_desc);

#endif