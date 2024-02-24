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

#include "optimized.hpp"

//======== GM ========================================================== 80 ====

parser::parser() {
  for (size_t i = 0; i < STATES; ++i) {
    
  }
}

void parser::accept(std::string in) {
  for (char& c : in) {
    switch (this->cur_state()) {

      default: 
        this->set_state(parser::state::REJECT);

    } 
  }
}

std::string parser::consume(std::string in) {

}

//======== GM ========================================================== 80 ====

mapped_file::~mapped_file() {
   delete this->map; 
}

mapped_file* map_file2mem(const char* path) {
  auto* file = new mapped_file();

  // try to open the file and if fail return nullptr for main to handle
  int fd; 
  if ((fd = open(path, O_RDONLY)) == -1 ||
      stat(path, &file->fileInfo) == -1) {
    return nullptr;
  }
  file->map = (char*)mmap(0, file->fileInfo.st_size, PROT_READ,
                                MAP_SHARED, fd, 0);
  if (file->map == MAP_FAILED) {
    return nullptr;
  }
  return file;
}

int main(int args, char** argv) {
   
   if (args == 1) {
      printf("No file input.\n");
      return 1;
   }

   auto* file = map_file2mem(argv[1]);

}

//======== GM ========================================================== 80 ====