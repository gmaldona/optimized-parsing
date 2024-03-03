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

//======== GM ========================================================== 80 ====

#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

//======== GM ========================================================== 80 ====

/*
 * Borrowed from Gregory Maldonado's GitHub Repository:
 * https://github.com/bu-cs447-2024-1s/one-billion-row-challenge-gmaldona
 * Representation for a mmap()'d file.
 */
class mapped_file {
 public:
  int fd;
  struct stat fileInfo;
  char *content;
  size_t size;
  long long offset = 0;

  ~mapped_file();
};

class String {

 private:
  char *str;
  size_t length = 0;

 public:
  String(size_t bufsize, char *key, char *value) {
     str = strcat(strcat(strcat(key, ","), value), "\n");
     length = bufsize + 2;
  }

  const char *c_str() {
     return str;
  }

  size_t size() const {
     return length;
  }
};

//======== GM ========================================================== 80 ====

/**
 * Parser for Strings and Integers. The Parser using a State Machine to parse,
 * see below.
 */
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

  /**
   * ASCII Bounds for Integers and Printable Characters.
   */
  enum ASCII {
    MIN = 32,  // min printable character.
    MAX = 126, // max printable character.
    IMIN = 48,  // min printable integer.
    IMAX = 57   // max printable integer.
  };

 public:
  // total number of states.
  static const size_t STATES = 6; // A Java reflection would be nice here hmm...
  static const size_t ACCEPTABLE = ASCII::MAX - ASCII::MIN; // printable chars
  static const size_t STRLEN = 20 + 1; // requirement: strlen is 20 + 1 byte \0

  parser();

  void set_state(parser::state s) {
     this->s = s;
  }

  parser::state cur_state() {
     return this->s;
  }

  /**
   * parsed character by character within the mmap'd file and uses a state
   * machine to parse and trie for storing parsed characters.
   * Prints the trie in the required format at the end of the function.
   * @param file a mmap'd file
   */
  void accept(mapped_file *file, mapped_file *out);

 private:
  parser::state s = parser::state::START;
  parser::state transition_table[STATES][ACCEPTABLE];

  size_t line = 1;

  char prev_char = '^';
  int prev_state = 1;
  int index = 0;
};

//======== GM ========================================================== 80 ====

class parse_trie {

 private:
  // considering the first index [0] to be a dead node.
  // if [0] then nothing found.

  const static int MAX_LEN = 20 + 1;
  const static int MAX_NODES = (10'000 + 1) * MAX_LEN;
  int NEXT = parser::ACCEPTABLE + 1;

 public:
  struct node {
    int next = -1;
    bool stored = false;
    char *key = new char[21];
    char *value = new char[65];
  };

  parse_trie();

  node **trie;

  /**
   * Inserts a key and value into the parse_trie
   * @param key: str
   * @param value: int
   */
  void insert(char *key, char *value);

  void traverse(const int index, mapped_file *out, size_t *offset);

  ~parse_trie();
};

//======== GM ========================================================== 80 ====

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
mapped_file *map_file2mem(const char *path);

/**
 * Borrowed from Gregory Maldonado's GitHub Repository:
 * https://github.com/bu-cs447-2024-1s/one-billion-row-challenge-gmaldona/blob/
 * 2008e90c7fe94a4d9515d09f94c597690e7a7e13/1brc.cpp#L49
 * Fixed Point String to Integer conversion.
 * 
 * @param p: const char* => string represnetation of an integer.
 * @returns int 
 */
inline int StoI(const char *p) {
   int x = 0;
   while (*p >= '0' && *p <= '9') {
      x = (x * 10) + (*p - '0');
      ++p;
   }
   return x;
}

/**
 * @param c: char
 * @return the integer representation of @param c but with the offset required
 * for the parser.
 */
inline size_t c2i(char c) {
   return (size_t) c - ' ';
}