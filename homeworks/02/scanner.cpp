#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <map>
#include <vector>

using namespace std;

enum Token {
    LPAR,
    RPAR,
    MUL,
    DIV,
    PLUS,
    SUB,
    EQ,
    SMICOLON,
    INT,
    VAR
};

bool S(const vector<Token> & tokens, int &i);
bool E(const vector<Token> & tokens, int &i);
bool Rhs(const vector<Token> & tokens, int &i);
bool Rhs2(const vector<Token> & tokens, int &i);

map<char, Token> tokens_map = {
    {'(', Token::LPAR},
    {')', Token::RPAR},
    {'*', Token::MUL},
    {'\\', Token::DIV},
    {'+', Token::PLUS},
    {'-', Token::SUB},
    {'=', Token::EQ},
    {';', Token::SMICOLON}
};

bool skip(char c) {
    return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}

vector<Token> scanner(const string & buf) {

    cout << "Parsing:" << endl;
    cout << buf << endl << endl;

    vector<Token> tokens;
    for (int i = 0; i < buf.length(); i++) {
        char c = buf[i];

        // char to skip
        if (skip(c)) {
            continue;
        }

        // check for single digit TOKENS
        map<char,Token>::iterator it = tokens_map.find(c);
        if (it != tokens_map.end()) {
            tokens.push_back(it->second);
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
            tokens.push_back(Token::INT);
            while(i < buf.length() && isdigit(buf[i])) i++;
        }
        else {
            tokens.push_back(Token::VAR);
            while(i < buf.length() && isalnum(buf[i])) i++;
        }
        i--;
    }

    return tokens;
}

bool parse(const vector<Token> & tokens) {
    int i = 0;
    bool res = S(tokens, i);
    return res && i == tokens.size();
}


bool S(const vector<Token> & tokens, int &i) {
    if (tokens[i++] != Token::VAR) return false;
    if (tokens[i++] != Token::EQ) return false;
    return E(tokens, i);
}

bool E(const vector<Token> & tokens, int &i) {
    if (tokens[i] == Token::INT) {
        i++;
        if (tokens[i++] != Token::SMICOLON) return false;

        return S(tokens, i);
    }

    if(!Rhs(tokens, i)) return false;
    return tokens[i++] == Token::SMICOLON;
}

bool Rhs(const vector<Token> & tokens, int &i) {
    Token next = tokens[i++];
    switch (next) {
        case Token::VAR:
            break;
        case Token::LPAR:
            if(!Rhs(tokens, i)) return false;
            if(tokens[i++] != Token::RPAR) return false;
            break;
        default:
            return false;
    }
    return Rhs2(tokens, i);
}


bool Rhs2(const vector<Token> & tokens, int &i) {
    Token next = tokens[i];
    if (!(next == Token::PLUS || next == Token::SUB || next == Token::DIV || next == Token::MUL)) return true; // accept empty
    i++;
    return Rhs(tokens,i) && Rhs2(tokens,i);

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

    vector<Token> tokens = scanner(buf);
    bool parsing = parse(tokens);

    if (parsing) cout << "Valid input" << endl;

    return 0;
}

