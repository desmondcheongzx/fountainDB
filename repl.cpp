#include "utils.h"
#define size_of_attribute(Struct, Attribute) sizeof(Struct::Attribute)

#define COLUMN_NAME_SIZE 32

typedef struct {
    uint_fast32_t id;
    char name[COLUMN_NAME_SIZE + 1];
} Row;

const uint_fast32_t ID_OFFSET = 0;
const uint_fast32_t ID_SIZE = size_of_attribute(Row, id);
const uint_fast32_t NAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint_fast32_t NAME_SIZE = size_of_attribute(Row, name);
const uint_fast32_t ROW_SIZE = ID_SIZE + NAME_SIZE;

void serialize_row(const Row& source, void* destination)
{
    memcpy(destination + ID_OFFSET, &(source.id), ID_SIZE);
    memcpy(destination + NAME_OFFSET, &(source.name), NAME_SIZE);
}

void deserialize_row(const void* source, Row& destination)
{
    memcpy(&(destination.id), source, ID_SIZE);
    memcpy(&(destination.name), source + NAME_OFFSET, NAME_SIZE);
}

// Each page stores 4 kB, similar to most virtual memory systems
const uint_fast32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint_fast32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint_fast32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Table {
public:
    Table() {
        pages.reserve(TABLE_MAX_PAGES);
    }
    uint_fast32_t num_rows = 0;
    std::vector<char*> pages;
    void free_table();
};

void Table::free_table()
{
    for (char* page : pages)
        delete[] page;
}

char* make_page()
{
    char* new_page = new char[PAGE_SIZE];
    return new_page;
}

void* row_slot(Table& table, uint_fast32_t row_num)
{
    uint_fast32_t page_num = row_num / ROWS_PER_PAGE;
    char* page;
    if (page_num >= table.pages.size()) {
        table.pages.push_back(make_page());
        page = table.pages.back();
    } else {
        page = table.pages.at(page_num);
    }
    uint_fast32_t row_offset = row_num % ROWS_PER_PAGE;
    uint_fast32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void print_row(const Row& row)
{
    std::cout << (uint_fast32_t) row.id << " ";
    std::cout << (std::string) row.name << std::endl;
}

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_FAIL
} ExecuteResult;

typedef enum {
    STATEMENT_SUCCESS,
    STATEMENT_SYNTAX_ERROR,
    STATEMENT_UNRECOGNIZED
} StatementResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SHOW,
    STATEMENT_DELETE
} StatementType;

typedef struct {
    StatementType type;
    Row row_info;
} Statement;

std::map <std::string, StatementType> statement_type_map = {
    {"insert", STATEMENT_INSERT},
    {"show", STATEMENT_SHOW},
    {"delete", STATEMENT_DELETE}
};

const std::string prompt = "fountainDB> ";

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
    Token (void) {}
    Token (char t) : type{t} {}
    Token (char t, std::string v) : type{t}, val{v} {
        // Normalize the stored string
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    }
    char type = TOKEN_UNSET;
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

MetaCommandResult execute_meta_command (Table& table, Token t)
{
    if (t.get_val() == "exit") {
        table.free_table();
        exit(0);
    }
    return META_COMMAND_UNRECOGNIZED;
}

ExecuteResult execute_insert(Table& table, const Row& row_info)
{
    if (table.num_rows >= TABLE_MAX_ROWS)
        return EXECUTE_TABLE_FULL;
    serialize_row(row_info, row_slot(table, table.num_rows));
    table.num_rows++;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_show(Table& table)
{
    Row row;
    for (uint_fast32_t i = 0; i < table.num_rows; i++) {
        deserialize_row(row_slot(table, i), row);
        print_row(row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Table& table)
{
    return EXECUTE_FAIL;
}


ExecuteResult dispatch_statement(Table& table, const Statement& instruction)
{
    switch (instruction.type) {
    case STATEMENT_INSERT:
        return execute_insert(table, instruction.row_info);
    case STATEMENT_SHOW:
        return execute_show(table);
    case STATEMENT_DELETE:
        return EXECUTE_FAIL;
    default:
        return EXECUTE_FAIL;
    }
    return EXECUTE_SUCCESS;
}

StatementResult prepare_statement(Statement& result, Token t)
{
    // TODO: implement statement syntax error
    std::string command = t.get_val();
    result.row_info.id = 0;
    result.row_info.name[0] = '\0';
    if (statement_type_map.find(command) == statement_type_map.end())
        return STATEMENT_UNRECOGNIZED;
    result.type = statement_type_map[command];
    switch (result.type) {
    case STATEMENT_INSERT:
        std::cin >> result.row_info.id >> result.row_info.name;
        return STATEMENT_SUCCESS;
    case STATEMENT_SHOW:
        return STATEMENT_SUCCESS;
    case STATEMENT_DELETE:
        // TODO
        return STATEMENT_SUCCESS;
    }
    return STATEMENT_UNRECOGNIZED;
}

void handle_input()
{
    TokenStream token_stream;
    Table table;
    while (std::cin) {
        std::cout << prompt;
        Token t = token_stream.get();
        if (t.type == TOKEN_UNSET)
            break;
        if (t.type == meta_prefix) {
            switch (execute_meta_command(table, t)) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED:
                std::cerr << "Command is not recognized.\n";
                continue;
            default:
                continue;
            }
        }

        Statement instruction;
        switch (prepare_statement(instruction, t)) {
        case STATEMENT_SUCCESS:
            break;
        case STATEMENT_SYNTAX_ERROR:
            std::cerr << "Syntax error for " + t.get_val() + " instruction.\n";
            continue;
        case STATEMENT_UNRECOGNIZED:
            std::cerr << "Unrecognized instruction: " + t.get_val() + ".\n";
            continue;
        }

        switch (dispatch_statement(table, instruction)) {
        case EXECUTE_SUCCESS:
            break;
        case EXECUTE_TABLE_FULL:
            std::cerr << "Error: Table full.\n";
            continue;
        case EXECUTE_FAIL:
            std::cerr << "Error: Execution failed.\n";
            continue;
        }
    }
    table.free_table();
}

int main()
{
    try {
        handle_input();
    }
    catch (std::string err) {
        std::cerr << err << std::endl;
    }
    return 0;
}
