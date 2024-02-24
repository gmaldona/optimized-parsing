/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream> 
#include <algorithm>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>

#include "standard.hpp"

using namespace std;

std::ostream& operator<<(std::ostream& oss, 
                         std::unordered_map<std::string, std::string> map) {
   for (auto& [key, value] : map) {
      oss << key << " " << value << endl;
   }
   return oss;
}

void parse(const string& line, unordered_map<string, vector<string>* >* parse_map) {
   const size_t  start = line.find("\""); 
   const size_t  end   = line.rfind("\""); 
   const string  key   = line.substr(start + 1, end); 
   const string substr = line.substr(end);

   regex  intexpr("[0-9]+"); 
   smatch match; 

   string value; 
   if (regex_search(substr, match, intexpr) ) {
      const string value = match.str(0); 

      if (parse_map->find(key) != parse_map->end()) {
         (*parse_map)[key]->push_back(value); 
      } else {
         (*parse_map)[key] = new vector<string>{ value }; 
      }
   }
}

unordered_map<string, string>
reduce(unordered_map<string, vector<string>* >* parse_map) {
   std::unordered_map<std::string, std::string> results {}; 

   for (auto& [key, vec] : *parse_map) {
      vector<int> mapped {};
      for (auto& strval : *vec) {
         const int value = atoi(strval.c_str());
         cout << value << endl;
         mapped.push_back(value);
      }

      results.insert( {
          key, 
          to_string(*max_element(mapped.begin(), mapped.end())) 
      });
   }

   return results; 
}

int main(int args, char** argv) {
   
   if (args == 1) {
      printf("No file input.\n");
      return 1;
   }

   auto* parse_map = new unordered_map<string, vector<string> *>{}; 
   string line;  
   ifstream file (argv[1]);
   while (getline(file, line)) {
      parse(line, parse_map); 
   }

   auto results = reduce(parse_map); 
   // cout << results;

   for (auto& [_, value] : *parse_map) {
      delete value;
   }

   // delete parse_map;
   return 0;
}