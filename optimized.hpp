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

class parser {
  /**    
   * STATE MACHINE: 
   * This state machine captures a string wrapped within quotations and a value
   * that consists of integers. 
   * 
   *      *[e]       *[ALPHA]       *[e]            *[0-9]
   *     (S_0) --"--> (S_1) --"--> (S_3) --[0-9]--> (S_4) --$--> END
   *                   |  ^\ 
   *                  (\)   (\ | " | .)
   *                   V     \ 
   *                 (S_2) ---\
   * 
   *  *[A] := State (S) does a transition [A] to state (S)
  */
  enum state {
    REJECT,       //     (0)
    START,        // S_0 (1)
    STR,          // S_1 (2)
    ESCAPED,      // S_2 (3)
    END_STR,      // S_3 (4)
    INT,          // S_4 (5)
  }; 

  enum ASCII {
     MIN = 32,
     MAX = 126,
    IMIN = 48,
    IMAX = 57
  };

public:
  static const size_t STATES = 6;
  static const size_t ACCEPTABLE = 94; 
  static const size_t STRLEN = 20 + 1; 

  parser(); 

  void set_state(parser::state s) {
    this->s = s;
  }

  parser::state cur_state() {
    return this->s;
  }

  void accept(mapped_file* file);

private:
  parser::state s = parser::state::START;
  parser::state transition_table[STATES][ACCEPTABLE];

  size_t line = 1; 

  char prev_char  = '^'; 
  int  prev_state = 1; 
  int  index      = 0; 
};

class parse_trie {

private:
  const static int MAX_NODES = 10'000;
  const static int MAX_LEN   = 20 + 1;
  int NEXT = parser::ACCEPTABLE;

  // [node_i][(int)char] = node_i+1
  // The first [0 - ACCEPTABLE-1] is reserverd
  int** trie;

public:
  parse_trie();
  void insert(char* key, char* value);
  ~parse_trie();
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

/**
 * Borrowed from Gregory Maldonado's GitHub Repository:
 * https://github.com/bu-cs447-2024-1s/one-billion-row-challenge-gmaldona/blob/
 * 2008e90c7fe94a4d9515d09f94c597690e7a7e13/1brc.cpp#L49
 * Fixed Point String to Integer conversion.
 * 
 * @param p: const char* p => string represnetation of an integer.
 * @returns int 
 */
inline int StoI(const char *p) {
    int x = 0;
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    return x;
}


inline size_t c2i(char c) {
  return (size_t) c - ' ';
}