/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <algorithm>
#include <fstream>
#include <regex>

#include "standard.hpp"

using namespace std;

//======== GM ========================================================== 80 ====

std::ostream& operator<<(std::ostream& oss, 
                         std::map<std::string, std::string, 
                                  std::less<std::string>> map) {
   for (auto& [key, value] : map) {
      oss << "\"" << key << "\" " << value << endl;
   }
   return oss;
}

void parse(const std::string& line, 
           std::map<std::string, std::vector<std::string>*, 
           std::less<std::string> >* parse_map) {
   const size_t  start = line.find("\""); 
   const size_t  end   = line.rfind("\"") - 1; 
   const string  key   = line.substr(start + 1, end - start); 
   const string substr = line.substr(end+1);

   // TODO: Error catching for incorrect parses. 

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

std::map<std::string, std::string>
reduce(std::map<std::string, std::vector<std::string>*, 
       std::less<std::string>>* parse_map) {
   std::map<std::string, std::string> results {}; 

   for (auto& [key, vec] : *parse_map) {
      vector<int> mapped {};
      for (auto& strval : *vec) {
         const int value = atoi(strval.c_str());
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

   auto* parse_map = new map<string, vector<string> *, less<string>>{}; 
   string line;  
   ifstream input (argv[1]);
   while (getline(input, line)) {
      parse(line, parse_map); 
   }

   auto results = reduce(parse_map); 
   cout << results;
   
   ofstream output (string(argv[1]).append("-results"));
   for (auto& [key, value] : results) {
      output << "\"" << key << "\" " << value << endl;
   }
   output.close();


   for (auto& [key, value] : *parse_map) {
      delete value;
   }

   delete parse_map;
   return 0;
}

//======== GM ========================================================== 80 ====