/*
 * define a map<string, map<string,string>> data structure on the shared memory to
 * store automata paths and corresponding prefixes
 */

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <iostream>
#include <stdint.h>

#ifndef SHARED_TABLE_H
#define SHARED_TABLE_H

using namespace boost::interprocess;


typedef managed_shared_memory::segment_manager                       segment_manager_t;
typedef allocator<void, segment_manager_t>                           void_allocator;
 
/*-- define string type --*/
typedef allocator<char, segment_manager_t>                           char_allocator;
typedef basic_string<char, std::char_traits<char>, char_allocator>   char_string;

/*-- define map<string, string> type --*/
typedef std::pair<const char_string, char_string> string_string_value_type;
typedef allocator<string_string_value_type, segment_manager_t> string_string_map_allocator;
typedef map<char_string, char_string, std::less<char_string>, string_string_map_allocator>  string_string_map;
 
/*-- define map<string, map<string, string>> --*/
typedef std::pair<const char_string, string_string_map> string_string_string_value_type;
typedef allocator<string_string_string_value_type, segment_manager_t> string_string_string_map_allocator;
typedef map<char_string, string_string_map, std::less<char_string>, string_string_string_map_allocator>  string_string_string_map;


const string shmId = "PathsTable";
const string tableName = "table_map";
const uint32_t size = 4096*1000*100;

#endif
