/*
* Graduate student @ Thomas J. Watson College of Engineering and Applied
* Sciences, Binghamton University.
*
* author: Gregory Maldonado
* email : gmaldonado@cs.binghamton.edu
* date  : 2024-02-14
*/

#include <sys/stat.h>

//======== GM ========================================================== 80 ====

struct MappedFile {
  struct stat fileInfo;
  char *map;
  ~MappedFile();
};

/**
 * Borrowed from Gregory Maldonado's GitHub Repository:
 * https://github.com/bu-cs447-2024-1s/one-billion-row-challenge-gmaldona 
 * /blob/2008e90c7fe94a4d9515d09f94c597690e7a7e13/1brc.cpp#L65
 * 
 * Map a file to memory to faster reads.
 *
 * @param path: const char* path =>  string representation of a path to a file.
 * 
 * @returns: a pointer to the file contents.
*/
MappedFile* map_file2mem(const char* path);

//======== GM ========================================================== 80 ====