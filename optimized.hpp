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
#include <stdlib.h>
#include <string.h>

class mapped_file {
public: 
  struct stat fileInfo;
  char *content;
  ~mapped_file();
};

/**
 * 
*/
class parser {

  enum state {
    REJECT,       //     (0)
    START,        // S_0 (1)
    STR,          // S_1 (2)
    ESCAPED,      // S_2 (3)
    END_STR,      // S_3 (4)
    INT,          // S_4 (5)
    END,          // S_5 (6)
  }; 

  enum ASCII {
     MIN = 32,
     MAX = 126,
    IMIN = 48,
    IMAX = 57
  };

private:
  static const size_t STATES = 7;
  static const size_t ACCEPTABLE = 94; 

  parser::state s = parser::state::START;
  parser::state transition_table[STATES][ACCEPTABLE];

  size_t line = 1; 

  char prev_char = '^'; 

public:
  parser(); 

  void set_state(parser::state s) {
    this->s = s;
  }

  parser::state cur_state() {
    return this->s;
  }

  void accept(mapped_file* file);
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

size_t c2i(char c) {
  return (size_t) c - 32;
}