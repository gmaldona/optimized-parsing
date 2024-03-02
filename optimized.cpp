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
    int next = 0;
    int i;
    node *n = &trie[key[0]][c2i(key[0])];
    for (i = 1; key[i] != '\0'; i++) {
        if (n->next == -1) {
            n->next = NEXT;
            next = NEXT++;
        } else {
            next = n->next;
        }
        n = &trie[next][c2i(key[i])];
    }

    int v = StoI(value);
    if (v > n->value) {
        n->stored = true;
        n->key = key;
        n->value = v;
    }
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

    transition_table[parser::state::START][c2i(' ')] = parser::state::START;
    transition_table[parser::state::START][c2i('\n')] = parser::state::START;
    transition_table[parser::state::START][c2i('"')] = parser::state::STR;

    transition_table[parser::state::STR][c2i('\\')] = parser::state::ESCAPED;

    transition_table[parser::state::ESCAPED][c2i('\\')] = parser::state::STR;
    transition_table[parser::state::ESCAPED][c2i('"')] = parser::state::STR;
    transition_table[parser::state::ESCAPED][c2i('.')] = parser::state::STR;

    transition_table[parser::state::STR][c2i('"')] = parser::state::END_STR;

    transition_table[parser::state::END_STR][c2i(' ')] = parser::state::END_STR;
}

void parser::accept(mapped_file *file) {

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

                    std::cout << key_str << ',' << value_str << std::endl;
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

        if (this->cur_state() == parser::state::START) {
            this->prev_char = '^';
            index = 0;
        } else {
            this->prev_char = c;
        }
        ++this->index;
    }

//    for (size_t i = 0; i < parser::ACCEPTABLE; ++i) {
//        parse_trie::node *n = &trie.trie[i][i];
//        while (n->next != -1) {
//            for (size_t alpha = 0; i < parser::ACCEPTABLE; ++i) {
//                if (n->stored) {
//
//                }
//                n = &trie.trie[n->next][alpha];
//            }
//        }
//    }
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

    parser p;
    p.accept(file);

}

//======== GM ========================================================== 80 ====