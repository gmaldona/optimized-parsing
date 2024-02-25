/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "optimized.hpp"
//======== GM ========================================================== 80 ====
parse_trie::parse_trie() {
  // the value of a node can store a signed integer.
  NEXT = parser::ACCEPTABLE; 
}

void parse_trie::insert(char* key, char* value, size_t keylen) {
  size_t next = 0; 
  for (size_t i = 0; i < keylen; ++i) {
    trie[next][c2i(key[i])] = NEXT;
    next = NEXT; NEXT++;
  }
  trie[next][0] = StoI(value);
}

//======== GM ========================================================== 80 ====

parser::parser() {
  for (size_t i = 0; i < STATES; i++) {
    memset(transition_table[i], parser::state::REJECT, ACCEPTABLE); 

    if (i == parser::state::STR) {
      for (char c = parser::ASCII::MIN; c <= parser::ASCII::MAX; ++c) {
        transition_table[parser::state::STR][c2i(c)] = parser::state::STR;
      }
    } else if (i == parser::state::END_STR || i == parser::state::INT) {
      for (char c = parser::ASCII::IMIN; c <= parser::ASCII::IMAX; ++c) {
        transition_table[i][c2i(c)] = parser::state::INT; 
      }
    }
  }

  transition_table[parser::state::START][c2i(' ')]  = parser::state::START;
  transition_table[parser::state::START][c2i('\n')] = parser::state::START;
  transition_table[parser::state::START][c2i('"')]  = parser::state::STR;
  
  transition_table[parser::state::STR][c2i('\\')] = parser::state::ESCAPED;
  transition_table[parser::state::ESCAPED][c2i('\\')] = parser::state::STR;
  transition_table[parser::state::ESCAPED][c2i('"')]  = parser::state::STR;
  transition_table[parser::state::ESCAPED][c2i('.')]  = parser::state::STR;

  transition_table[parser::state::STR][c2i('"')]  = parser::state::END_STR;

  transition_table[parser::state::END_STR][c2i(' ')] = parser::state::END_STR; 
}

void parser::accept(mapped_file* file) {
  char* chars = file->content; 

  char key[20], value[20];
  size_t char_p = 0;
  size_t value = 0; 
  
  for (off_t i = 0; i <= file->fileInfo.st_size; ++i) {
    char c = chars[i]; 
    this->prev_state = this->cur_state();
    std::cout << cur_state() << " => "; 
    switch (this->cur_state()) {
      case parser::state::START:
        this->set_state(transition_table[parser::state::START][c2i(c)]); 
        break;
      case parser::state::STR:
        key[char_p] = c;
        char_p++;
        this->set_state(transition_table[parser::state::STR][c2i(c)]);
        break;
      case parser::state::ESCAPED:
        key[char_p] = '\\';
        char_p++;
        this->set_state(transition_table[parser::state::ESCAPED][c2i(c)]);
        break;
      case parser::state::END_STR:
        this->set_state(transition_table[parser::state::END_STR][c2i(c)]);
        char_p = 0;
        break;
      case parser::state::INT:
        // c2i(x) => x - 32. NL is 0x10, 0x10 - 32 == ??? 
        if (c == '\n' || c == ' ') { 
          this->set_state(parser::state::START);
          ++this->line;
          this->index = 0; 
        } else {
          value[char_p] = c;
          char_p++;
          this->set_state(transition_table[parser::state::INT][c2i(c)]);  
        }
        break;
      case parser::state::REJECT: 
        printf("Failed to parse. Parse Exception on line %zu.\n", this->line);
        printf("prev_char='%c', failed on '%c' in state: %d at index=%d.\n", 
               this->prev_char,
               c,
               this->prev_state,
               this->index); 
        exit(1);

      default: 
        this->set_state(parser::state::REJECT);
    } 

    std::cout << "' " << c << " '"<< " => " << this->cur_state() << std::endl;

    if (this->cur_state() == parser::state::START) {
      this->prev_char  = '^';
      index = 0; 
    } else {
      this->prev_char = c; 
    } 
    ++this->index;
  }
}

//======== GM ========================================================== 80 ====

mapped_file::~mapped_file() {
   delete this->content; 
}

mapped_file* map_file2mem(const char* path) {
  auto* file = new mapped_file();

  // try to open the file and if fail return nullptr for main to handle
  int fd; 
  if ((fd = open(path, O_RDONLY)) == -1 ||
      stat(path, &file->fileInfo) == -1) {
    return nullptr;
  }
  file->content = (char*)mmap(0, file->fileInfo.st_size, PROT_READ,
                                MAP_SHARED, fd, 0);
  if (file->content == MAP_FAILED) {
    return nullptr;
  }
  return file;
}

int main(int args, char** argv) {
   
  if (args == 1) {
    printf("No file input.\n");
    return 1;
  }

  auto file = map_file2mem(argv[1]);
  
  parser p; 
  p.accept(file);

}

//======== GM ========================================================== 80 ====