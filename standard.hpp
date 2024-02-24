/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <iostream>


void parse(const std::string& line, 
           std::unordered_map<std::string, 
                              std::vector<std::string>* >* parse_map);

std::unordered_map<std::string, std::string>
reduce(std::unordered_map<std::string, std::vector<std::string>* >* parse_map); 

std::ostream& operator<<(std::ostream& oss, 
                         std::unordered_map<std::string, std::string> map); 