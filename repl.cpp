#include "utils.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED
} PrepareResult;

class Token {
public:
    Token (void) {}
    Token (char t) : type{t} {}
    Token (char t, std::string v) : type{t}, val{v} {
        // Normalize the stored string
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    }
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
    std::string val;
    std::cin >> ch;
    switch (ch) {
    case '.': // meta command
        std::cin >> val;
        return Token{ch, val};
    case ';':
        return Token{ch};
    default:
        std::cin.putback(ch);
        std::cin >> val;
        return Token{'$', val};
    }
    return Token{';'};
}

void TokenStream::putback (Token t)
{
    if (buffer_filled)
        error("Your buffer is full");
    buffer = t;
    buffer_filled = true;
}

MetaCommandResult execute_meta_command (Token t)
{
    if (t.val == "exit") {
        exit(0);
    }
    return META_COMMAND_UNRECOGNIZED;
}

TokenStream token_stream;

int main()
{
    while (std::cin) {
        try {
            std::cout << "fountainDB> ";
            Token t = token_stream.get();
            if (t.type == '.') {
                switch (execute_meta_command(t)) {
                case (META_COMMAND_SUCCESS):
                    break;
                case (META_COMMAND_UNRECOGNIZED):
                    error("Command is not recognized.\n");
                }
                continue;
            }
            std::cout << t.val << std::endl;
        }
        catch (std::string err) {
            std::cerr << err << std::endl;
        }
    }
    return 0;
}
