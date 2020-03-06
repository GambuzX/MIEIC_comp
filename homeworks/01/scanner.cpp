#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <map>

using namespace std;

map<char, string> tokens_map = {
    {'(', "LPAR"},
    {')', "RPAR"},
    {'*', "MUL"},
    {'\\', "DIV"},
    {'+', "PLUS"},
    {'-', "SUB"},
    {'=', "EQ"},
    {';', "SMICOLON"}
};

bool skip(char c) {
    return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}

int main(int argc, char * argv[]) {
    if (argc != 3 || (string(argv[1]) != "file" && string(argv[1]) != "string")) {
        cout << "Usage: <filename> <type> <content>" << endl;
        cout << "type is either 'string' or 'file'" << endl;
        cout << "content is the string value or the filename" << endl;
        return 1;
    }

    string buf;
    if (string(argv[1]) == "string") buf = string(argv[2]);
    else {
        ifstream ifile(argv[2]);
        buf = string((istreambuf_iterator<char>(ifile)),
                    istreambuf_iterator<char>());
    }

    cout << "Parsing:" << endl;
    cout << buf << endl << endl;

    string tokens = "";
    for (int i = 0; i < buf.length(); i++) {
        char c = buf[i];

        // char to skip
        if (skip(c)) {
            tokens += c;
            continue;
        }

        // check for single digit TOKENS
        map<char,string>::iterator it = tokens_map.find(c);
        if (it != tokens_map.end()) {
            tokens += "<" + it->second + "> ";
            continue;
        }

        // unknown char
        if (!isalnum(c)) {
            cout << "Found unknown char " << c << endl;
            cout << "Exiting..."<< endl;
            exit(1);
        }

        // decide between INT and VAR and consume next chars
        if (isdigit(c)) {
            tokens += "<INT> ";
            while(i < buf.length() && isdigit(buf[i])) i++;
        }
        else {
            tokens += "<VAR> ";
            while(i < buf.length() && isalnum(buf[i])) i++;
        }
        i--;
    }

    cout << "Tokens:" << endl;
    cout << tokens << endl;
    return 0;
}

