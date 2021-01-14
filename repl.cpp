#include "utils.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum {
    STATEMENT_SUCCESS,
    STATEMENT_UNRECOGNIZED
} StatementResult;

typedef enum {
    INSERT,
    SHOW
} StatementType;

std::map <std::string, StatementType> statement_type_map = {
    {"insert", INSERT},
    {"show", SHOW}
};

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
    std::string get_val();
};

std::string Token::get_val ()
{
    switch (type) {
    case '.':
    case '$':
        return val;
    default:
        error("Token type has no value\n");
    }
    return "";
}

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
    if (t.get_val() == "exit") {
        exit(0);
    }
    return META_COMMAND_UNRECOGNIZED;
}

TokenStream token_stream;
std::vector<std::string> table;

StatementResult dispatch_statement (Token t)
{
    std::string command = t.get_val();
    std::string entry;

    if (statement_type_map.find(command) == statement_type_map.end()) {
        error("Command not known\n");
    }
    switch (statement_type_map[command]) {
    case INSERT:
        std::cin >> entry;
        table.push_back(entry);
        return STATEMENT_SUCCESS;
    case SHOW:
        for (std::vector<std::string>::iterator vi = table.begin();
             vi != table.end(); vi++) {
            std::cout << *vi << std::endl;
        }
        return STATEMENT_SUCCESS;
    }
    return STATEMENT_UNRECOGNIZED;
}

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
            dispatch_statement(t);
        }
        catch (std::string err) {
            std::cerr << err << std::endl;
        }
    }
    return 0;
}
