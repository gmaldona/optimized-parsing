/**
 * THIS CODE WAS TAKEN FROM https://piazza.com/class/lrkxtzhdnc831h/post/80. 
 * THIS IS CODE PROVIDED BY THE PROFESSOR, KENNETH CHIU, TO ASSIST WITH
 * GENERATING TEST RESULTS FOR ASSIGNMENT 2. 
*/

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iomanip> // Required for std::setw and std::setfill

using namespace std;

class RandomGenerator {
public:
    default_random_engine eng{random_device{}()};
    uniform_int_distribution<int> str_len{10, 20};
    uniform_int_distribution<int> char_dist{32, 126};
    uniform_int_distribution<int> spaceDist{0, 10};


    char getRandomChar() {
        char randomChar;
        do {
            randomChar = static_cast<char>(char_dist(eng));

        } while (randomChar == '\"' || randomChar == '.');
        return randomChar;
    }

    string generateRandomString() {
        int length = str_len(eng);
        string str;
        str.reserve(length);
        for (int i = 0; i < length; ++i) {

            char randomChar = getRandomChar();

            if (randomChar == '\\') { // Ensure there's space for 2 characters
                if (i >= length - 1) {
                    continue;
                }
                str += randomChar; // Add the first '\'
                str += getRandomSpecialChar(); // Add the second character, which is either '\', '.', or '"'
                i++; // Increment i since we've added two characters
            } else {
                str += randomChar;
            }
        }
        return str;
    }


    const string &generateRandomSpace() {
        static vector<string> vs {
            "",
            " ", 
            "  ", 
            "   ", 
            "    ",
            "     "
        };
        int r = spaceDist(eng);
        if (r >= vs.size()) {
            return vs[0];
        }
        return vs[r];
    }

private:
    const string allowedChars = "\\\"";
    uniform_int_distribution<int> specialCharDist{0, 1};

    char getRandomSpecialChar() {
        return allowedChars[specialCharDist(eng)];
    }
};

int main(int argc, char const *argv[]) {
    size_t n_line = 10;
    if (argc >= 2) {
        n_line = stoul(argv[1]);
    }
    const int numStrings = std::min<int>(n_line, 10000); // Use n_line instead of a fixed number
    vector<string> randomStrings;
    RandomGenerator randGen;

    // Generate n_line random strings
    for (int i = 0; i < numStrings; ++i) {
        randomStrings.push_back(randGen.generateRandomString());
    }
    uniform_int_distribution<> intDist(0, 1000000000);
    uniform_int_distribution<> strDist(0, numStrings - 1);
    uniform_int_distribution<> zeroDist(1, 3);


    for (size_t i = 0; i < n_line; ++i) {
        int randomInt = intDist(randGen.eng);
        int str_id =  strDist(randGen.eng);
        int r = zeroDist(randGen.eng);
        if ( r == 1) {
            cout << "\"" << randomStrings[str_id] << "\"" << randGen.generateRandomSpace() << std::setfill('0') << std::setw(11) << randomInt << endl;
        } else {
            cout << "\"" << randomStrings[str_id] << "\"" << randGen.generateRandomSpace() << randomInt << endl;
        }
    }

    return 0;
}