/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*
* Copyright 2024 GREGORY MALDONADO
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <algorithm>
#include <fstream>
#include <regex>
#include <chrono>

#include "standard.hpp"

using namespace std;
using namespace std::chrono;

//======== GM ========================================================== 80 ====

std::ostream &operator<<(std::ostream &oss,
                         std::map<std::string, std::string,
                                  std::less<std::string>> map) {
   for (auto &[key, value] : map) {
      oss << "\"" << key << "\" " << value << endl;
   }
   return oss;
}

void parse(const std::string &line,
           std::map<std::string, std::vector<std::string> *,
                    std::less<std::string> > *parse_map) {
   const size_t start = line.find('\"');
   const size_t end = line.rfind('\"') - 1;
   const string key = line.substr(start + 1, end - start);
   const string substr = line.substr(end + 1);

   // TODO: Error catching for incorrect parses. 

   regex intexpr("[0-9]+");
   smatch match;

   string value;
   if (regex_search(substr, match, intexpr)) {
      const string value = match.str(0);

      if (parse_map->find(key) != parse_map->end()) {
         (*parse_map)[key]->push_back(value);
      } else {
         (*parse_map)[key] = new vector<string>{value};
      }
   }
}

std::map<std::string, std::string>
reduce(std::map<std::string, std::vector<std::string> *,
                std::less<std::string>> *parse_map) {
   std::map<std::string, std::string> results{};

   for (auto &[key, vec] : *parse_map) {
      vector<string> mapped{};
      for (auto &strval : *vec) {
         mapped.push_back(strval);
      }

      string max_str;
      int max = 0;
      for (auto &v : mapped) {
         const int value = atoi(v.c_str());
         if (value > max) {
            max_str = v;
            max = value;
         }
      }

      results.insert({key, max_str});
   }

   return results;
}

int main(int args, char **argv) {

   if (args == 1) {
      cout << "No file input." << endl;
      return 1;
   }

   steady_clock::time_point begin = steady_clock::now();

   auto *parse_map = new map<string, vector<string> *, less<string>>{};
   string line;
   ifstream input(argv[1]);
   while (getline(input, line)) {
      parse(line, parse_map);
   }

   auto results = reduce(parse_map);
   // cout << results;

   ofstream output(string(argv[1]).append("-results"));
   for (auto &[key, value] : results) {
      output << "\"" << key << "\"," << value << endl;
   }

   for (auto &[key, value] : *parse_map) {
      delete value;
   }

   steady_clock::time_point end = steady_clock::now();
   std::cout << duration_cast<nanoseconds>(end - begin).count()
             << "[ms]" << std::endl;

   output.close();

   delete parse_map;
   return 0;
}

//======== GM ========================================================== 80 ====