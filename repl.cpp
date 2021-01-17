#include "utils.h"
#include "token.h"
#include "table.h"

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

MetaCommandResult execute_meta_command(Table& table, Token t)
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
    serialize_row(row_info, table.row_slot(table.num_rows));
    table.num_rows++;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_show(Table& table)
{
    Row row;
    for (uint_fast32_t i = 0; i < table.num_rows; i++) {
        deserialize_row(table.row_slot(i), row);
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
