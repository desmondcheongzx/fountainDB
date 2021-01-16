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
    SHOW,
    DELETE
} StatementType;

const std::string prompt = "fountainDB> ";

// meta commands start with this char
const char meta_prefix = '.';

// tokens with this char as their type are normal statements
const char statement_prefix = '$';

// a complete query should end with this symbol
const char terminating_symbol = ';';

std::map <std::string, StatementType> statement_type_map = {
    {"insert", INSERT},
    {"show", SHOW},
    {"delete", DELETE}
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
    case meta_prefix:
    case statement_prefix:
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
    case DELETE:
        std::cin >> entry;
        for (std::vector<std::string>::iterator vi = table.begin();
             vi != table.end(); vi++) {
            if (*vi == entry) {
                table.erase(vi);
                return STATEMENT_SUCCESS;
            }
        }
        error("Entry for " + entry + " was not found");
    }
    return STATEMENT_UNRECOGNIZED;
}

int main()
{
    TokenStream token_stream;
    while (std::cin) {
        try {
            std::cout << prompt;
            Token t = token_stream.get();
            if (t.type == meta_prefix) {
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
