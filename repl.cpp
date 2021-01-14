#include "utils.h"

class Token {
public:
    char type;
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

Token TokenStream::get ()
{
    if (buffer_filled) {
        buffer_filled = false;
        return buffer;
    }
    char ch;
    std::cin >> ch;
    switch (ch) {
    case ';':
    case 'q':
        return Token{ch};
    default:
        std::cin.putback(ch);
        std::string val;
        std::cin >> val;
        return Token{'.', val};
    }
    return Token{'q'};
}

void TokenStream::putback (Token t)
{
    if (buffer_filled)
        error("Your buffer is full");
    buffer = t;
    buffer_filled = true;
}

TokenStream token_stream;

int main()
{
    while (std::cin) {
        try {
            std::cout << "fountainDB> ";
            Token t = token_stream.get();
            if (t.type == 'q')
                break;
            std::cout << t.val << std::endl;
        }
        catch (std::string err) {
        }
    }
    return 0;
}
