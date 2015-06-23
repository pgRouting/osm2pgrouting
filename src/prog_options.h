#ifndef _prog_options_h
#define _prog_options_h

void get_option_description(po::options_description &od_desc);
int process_command_line(po::variables_map &vm,po::options_description &od_desc);

#endif