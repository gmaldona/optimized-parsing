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

#include "optimized.hpp"

//======== GM ========================================================== 80 ====

parse_trie::parse_trie() {
   // the value of a node can store a signed integer.
   trie = new node *[MAX_NODES];

   for (size_t i = 0; i < MAX_NODES; i++) {
      trie[i] = new node[parser::ACCEPTABLE];
      memset(trie[i], 0, parser::ACCEPTABLE);
   }
}

void parse_trie::insert(char *key, char *value) {

   int i = 0;
   int index = c2i(key[0]); // OFFSET OF 1
   int next = index;
   node *n = &trie[index + 1][index];
   while (key[i] != '\0') {

      // std::cout << "[" << next << "]" << "[" << c2i(key[i])
      //          << "(" << key[i] << ")] = " << NEXT << std::endl;

      if (n->next > 0) {
         next = n->next;
      } else if (n->next < 1 && key[i + 1] != '\0') {
         n->next = NEXT;
         next = NEXT;
         NEXT++;
      }

      i++;
      if (key[i] != '\0') {
         n = &trie[next][c2i(key[i])];
      }
   }
   n = &trie[next][c2i(key[i - 1])];
   // std::cout << "next=" << next << ", index=" << c2i(key[i - 1]) << std::endl;
   int v = StoI(value);
   if (n->value == nullptr || v > StoI(n->value)) {
      n->stored = true;
      strcpy(n->key, key);
      strcpy(n->value, value);
      // std::cout << "key=" << n->key << ",value=" << n->value << std::endl;
   }
}

size_t parse_trie::traverse(const int index, mapped_file *out, size_t offset) {
   for (size_t i = 0; i < parser::ACCEPTABLE; i++) {
      node *n = &trie[index][i];
      if (n->next > 0) {
         traverse(n->next, out, offset);
      }
      if (n->stored) {
         size_t length = strlen(n->key) + strlen(n->value);
         String str(length, n->key, n->value);
         memcpy(out->content + offset, str.c_str(), str.size());
         offset += str.size();
//         std::cout << str.c_str();
      }
   }

   return offset;
}

parse_trie::~parse_trie() {
   for (size_t i = 0; i < MAX_NODES; i++) {
      delete[] trie[i];
   }
   delete[] trie;
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

   transition_table[state::START][c2i(' ')] = parser::state::START;
   transition_table[state::START][c2i('\n')] = parser::state::START;
   transition_table[state::START][c2i('"')] = parser::state::STR;

   transition_table[state::STR][c2i('\\')] = parser::state::ESCAPED;

   transition_table[state::ESCAPED][c2i('\\')] = parser::state::STR;
   transition_table[state::ESCAPED][c2i('"')] = parser::state::STR;
   transition_table[state::ESCAPED][c2i('.')] = parser::state::STR;

   transition_table[state::STR][c2i('"')] = parser::state::END_STR;

   transition_table[state::END_STR][c2i(' ')] = parser::state::END_STR;
}

void parser::accept(mapped_file *file, mapped_file *out) {

   parse_trie trie;
   char *chars = file->content;
   char key_str[parser::STRLEN];
   char value_str[parser::STRLEN];
   memset(key_str, 0, parser::STRLEN);
   memset(value_str, 0, parser::STRLEN);
   size_t char_p = 0;
   size_t val_p = 0;

   for (off_t i = 0; i <= file->fileInfo.st_size; ++i) {
      char c = chars[i];
      this->prev_state = this->cur_state();
      switch (this->cur_state()) {
         case parser::state::START:
            this->set_state(transition_table[parser::state::START][c2i(c)]);
            break;
         case parser::state::STR:
            if (c != '\"' || this->prev_state == parser::state::ESCAPED) {
               key_str[char_p] = c;
               char_p++;
            }
            this->set_state(transition_table[parser::state::STR][c2i(c)]);
            break;
         case parser::state::ESCAPED:
            key_str[char_p] = c;
            char_p++;
            this->set_state(transition_table[parser::state::ESCAPED][c2i(c)]);
            break;
         case parser::state::END_STR:
            key_str[char_p] = '\0';
            if (49 <= c && c <= 57) { // isa_int
               value_str[val_p] = c;
               val_p++;
            }
            this->set_state(transition_table[parser::state::END_STR][c2i(c)]);
            break;
         case parser::state::INT:
            // c2i(x) => x - 32. NL is 0x10, 0x10 - 32 == ???
            if (c == '\n' || c == ' ' || c == '\t') {
               value_str[val_p] = '\0';

               trie.insert(key_str, value_str);

               this->set_state(parser::state::START);
               ++this->line;
               this->index = 0;

               // std::cout << key_str << ',' << value_str << std::endl;
               char_p = 0;
               val_p = 0;

               memset(key_str, 0, sizeof(key_str));
               memset(value_str, 0, sizeof(value_str));

            } else {
               value_str[val_p] = c;
               val_p++;
               this->set_state(transition_table[parser::state::INT][c2i(c)]);
            }
            break;
         case parser::state::REJECT:
            printf("Failed to parse. Parse Exception on line %zu.\n",
                   this->line);
            printf("prev_char='%c', failed on '%c' in state: %d at index=%d.\n",
                   this->prev_char,
                   c,
                   this->prev_state,
                   this->index);
            exit(1);

         default:
            this->set_state(parser::state::REJECT);
      }

      if (this->cur_state() == parser::state::START) {
         this->prev_char = '^';
         index = 0;
      } else {
         this->prev_char = c;
      }
      ++this->index;
   }

   size_t offset = 0;
   for (int alpha = 0; alpha < parser::ACCEPTABLE; ++alpha) {
      offset = offset + trie.traverse(alpha + 1,
                                      out,
                                      offset);
   }
   std::cout << out->content << std::endl;
   msync(out->content, out->size, MS_SYNC);
}

//======== GM ========================================================== 80 ====

mapped_file::~mapped_file() {
   delete this->content;
}

mapped_file *map_file2mem(const char *path) {
   auto *file = new mapped_file();

   // try to open the file and if fail return nullptr for main to handle
   int fd;
   if ((fd = open(path, O_RDONLY)) == -1 ||
      stat(path, &file->fileInfo) == -1) {
      return nullptr;
   }
   file->content = (char *) mmap(0, file->fileInfo.st_size, PROT_READ,
                                 MAP_SHARED, fd, 0);
   if (file->content == MAP_FAILED) {
      return nullptr;
   }
   return file;
}

int main(int args, char **argv) {
   if (args == 1) {
      printf("No file input.\n");
      return 1;
   }

   auto file = map_file2mem(argv[1]);
   auto out = new mapped_file();

   out->fd = open(strcat(argv[1], "-results"),
                  O_RDWR | O_CREAT, (mode_t) 0600);
   out->content = (char *) mmap(NULL, file->fileInfo.st_size,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                out->fd, 0);
   out->size = file->fileInfo.st_size;
   parser p;
   p.accept(file, out);

   close(out->fd);
   munmap(out->content, file->fileInfo.st_size);
}

//======== GM ========================================================== 80 ====