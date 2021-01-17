#ifndef TOKEN_H
#define TOKEN_H

// meta commands start with this char
const char meta_prefix = '.';

// tokens with this char as their type are normal statements
const char statement_prefix = '$';

// a complete query should end with this symbol
const char terminating_symbol = ';';

// unset tokens have this type
const char TOKEN_UNSET = '\0';

class Token {
public:
    Token();
    Token(char t);
    Token(char t, std::string v);
    char get_type();
    std::string get_val();
private:
    char type = TOKEN_UNSET;
    std::string val = "";
};

class TokenStream {
public:
    Token get();
    void putback(Token t);
private:
    Token buffer;
    bool buffer_filled = false;
};

#endif
