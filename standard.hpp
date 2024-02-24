/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <iostream>
#include <map>

/**
 * Parses a single line of text into the "key" part and the "value" part. The
 * requirements are that the key is a string wrapped within quotations. The
 * value part is an integer value with r'[0]*' leading and/or trailing. 
 * #parse(string, map<string, vector<string>*>*) modifies the
 * unordred_map parameter.
 * This implementation is deemed "standard" and utilized the C++ standard lib
 * for computation. Although the C++ standard lib is convient, there is massive
 * overhead. This implementation could be deemed "slow" for the C++ standard lib
 * overhead. 
 * 
 * @param line:      const std::string& => text to be parsed.
 * 
 * @param parse_map: std::map<std::string, 
                                        std::vector<std::string>* >* 
                     => map containing parsed lined.

   @note: parse_map is mutated. 
*/
void parse(const std::string& line, 
           std::map<std::string, std::vector<std::string>*, 
           std::less<std::string> >* parse_map);

std::map<std::string, std::string>
reduce(std::map<std::string, std::vector<std::string>*, 
       std::less<std::string>>* parse_map); 

std::ostream& operator<<(std::ostream& oss, 
                         std::map<std::string, std::string, 
                                  std::less<std::string>> map); 