#include "utils.h"
#include "token.h"

Token::Token(void) {}

Token::Token(char t)
    :type{t}
{
}

Token::Token (char t, std::string v)
    :type{t}, val{v}
{
    // Normalize the stored string
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
}

char Token::get_type()
{
    return type;
}

std::string Token::get_val()
{
    switch (type) {
    case meta_prefix:
    case statement_prefix:
        return val;
    default:
        error("Token type has no value\n");
    }
    return "";
}

Token TokenStream::get()
{
    if (buffer_filled) {
        buffer_filled = false;
        return buffer;
    }
    char ch;
    std::string val;

    std::cin >> ch;
    switch (ch) {
    case meta_prefix:
        std::cin >> val;
        return Token{ch, val};
    case terminating_symbol:
        return Token{ch};
    default:
        std::cin.putback(ch);
        std::cin >> val;
        return Token{statement_prefix, val};
    }
    return Token{terminating_symbol};
}

void TokenStream::putback(Token t)
{
    if (buffer_filled)
        error("Your buffer is full");
    buffer = t;
    buffer_filled = true;
}
