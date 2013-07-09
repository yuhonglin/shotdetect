/**
 * @file   option.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Tue Jul  9 14:48:59 2013
 * 
 * @brief  declare the option class
 * 
 * 
 */

#ifndef _OPTION_H_
#define _OPTION_H_

#include <map>
#include <string>
#include <set>
#include "logger.hpp"

using namespace std;

namespace dms { // this is part of the Data Mining Studio project

  class Option
  {
  public:
    Option();
    virtual ~Option();

    /** 
     * set the head of the help info (printed before options infomation)
     * 
     * @param h 
     */
    void set_head(string h);
    /** 
     * set the tail of the help info (printed after options infomation)
     * 
     * @param t 
     */
    void set_tail(string t);

    // interface for float
    void add_float(string key, string comment, float defval, set< float > domain = set< float >());
    void add_float(string key, string comment);
    float get_float(string key);

    // interface for int
    void add_int(string key, string comment, int defval, set< int > domain = set< int >());
    void add_int(string key, string comment);
    int get_int(string key);

    // interface for double
    void add_double(string key, string comment, double defval, set< double > domain = set< double >());
    void add_double(string key, string comment);
    double get_double(string key);

    // interface for string
    void add_string(string key, string comment, string defval, set< string > domain = set< string >());
    void add_string(string key, string comment);
    string get_string(string key);

    // interface for bool
    void add_bool(string key, string comment, bool defval = false); // false means "no such option specified"
    bool get_bool(string key);

    // parse the argv, treating every as string
    bool parse(int argc, char** argv, bool allowunknownoption = false);

    // print help info
    void dump_help();
    
  private:
    string head;
    string tail;
    map< string, string > key_defval;
    map< string, string > key_value;
    map< string, string > key_type;
    map< string, string > key_comment;
    map< string, set < string > > key_domain;
    set< string > key_musthave;
    int num_opt;

  };
  
}

#endif /* _OPTION_H_ */
