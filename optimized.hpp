/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <sys/stat.h>
#include <iostream>

class mapped_file {
public: 
  struct stat fileInfo;
  char *map;
  ~mapped_file();
};

/**
 * 
*/
class parser {

  enum state {
    KEY,
    ESCAPED,
    VALUE,
    REJECT,
  }; 

  enum transition {
    EPSILON, // white space state
    ESCAPE,
    ALPHA, 
    NUMERAL
  };

  class parse_exception : public std::exception {

    private: 
      size_t line; 

    public:
    parse_exception(size_t line) : line(line) {}

    virtual const char * what() {
      return ("Parse Exception on line" + std::to_string(line)).c_str();
    }
  };

private:
  static const size_t STATES = 4;
  static const size_t ACCEPTABLE = 95; 

  parser::state s;
  parser::state transition_table[STATES][ACCEPTABLE];

public:
  parser(); 

  void set_state(parser::state s) {
    this->s = s;
  }

  parser::state cur_state() {
    return this->s;
  }

  void accept(std::string in);

  std::string consume(std::string in);

};

/**
 * Borrowed from Gregory Maldonado's GitHub Repository:
 * https://github.com/bu-cs447-2024-1s/one-billion-row-challenge-gmaldona
 * /blob/2008e90c7fe94a4d9515d09f94c597690e7a7e13/1brc.cpp#L65
 * 
 * Map a file to memory to faster reads.
 *
 * @param path: const char* path =>  string representation of a path to a file.
 * @returns: a pointer to the file contents.
*/
mapped_file* map_file2mem(const char* path);