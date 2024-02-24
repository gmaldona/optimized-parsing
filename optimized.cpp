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

MappedFile::~MappedFile() {
   delete this->map; 
}

MappedFile* map_file2mem(const char* path) {
  auto mappedFile = new MappedFile();

  // try to open the file and if fail return nullptr for main to handle
  int fd; 
  if ((fd = open(path, O_RDONLY)) == -1 ||
      stat(path, &mappedFile->fileInfo) == -1) {
    return nullptr;
  }
  mappedFile->map = (char*)mmap(0, mappedFile->fileInfo.st_size, PROT_READ,
                                MAP_SHARED, fd, 0);
  if (mappedFile->map == MAP_FAILED) {
    return nullptr;
  }
  return mappedFile;
}

int main(int args, char** argv) {
   
   if (args == 1) {
      printf("No file input.\n");
      return 1;
   }

   auto* mapped_file = map_file2mem(argv[1]);

}

//======== GM ========================================================== 80 ====