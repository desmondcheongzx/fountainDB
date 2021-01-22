#include "utils.h"
#include "token.h"
#include "table.h"

enum class MetaCommandResult
{
    SUCCESS,
    UNRECOGNIZED
};

enum class ExecuteResult
{
    SUCCESS,
    TABLE_FULL,
    FAIL
};

enum class StatementResult
{
    SUCCESS,
    SYNTAX_ERROR,
    UNRECOGNIZED
};

enum class StatementType
{
    INSERT,
    SHOW,
    DELETE
};

typedef struct {
    StatementType type;
    Row row_info;
} Statement;

std::map <std::string, StatementType> statement_type_map = {
    {"insert", StatementType::INSERT},
    {"show", StatementType::SHOW},
    {"delete", StatementType::DELETE}
};

const std::string prompt = "fountainDB> ";

MetaCommandResult execute_meta_command(Table& table, Token t)
{
    if (t.get_val() == "exit") {
        table.db_close();
        exit(0);
    }
    return MetaCommandResult::SUCCESS;
}

ExecuteResult execute_insert(Table& table, const Row& row_info)
{
    if (table.num_rows() >= TABLE_MAX_ROWS)
        return ExecuteResult::TABLE_FULL;
    serialize_row(row_info, table.row_slot(table.num_rows()));
    table.inc_rows();
    return ExecuteResult::SUCCESS;
}

ExecuteResult execute_show(Table& table)
{
    Row row;
    for (uint_fast32_t i = 0; i < table.num_rows(); i++) {
        deserialize_row(table.row_slot(i), row);
        print_row(row);
    }
    return ExecuteResult::SUCCESS;
}

ExecuteResult execute_delete(Table& table)
{
    return ExecuteResult::FAIL;
}


ExecuteResult dispatch_statement(Table& table, const Statement& instruction)
{
    switch (instruction.type) {
    case (StatementType::INSERT):
        return execute_insert(table, instruction.row_info);
    case (StatementType::SHOW):
        return execute_show(table);
    case (StatementType::DELETE):
        return ExecuteResult::FAIL;
    default:
        return ExecuteResult::FAIL;
    }
    return ExecuteResult::SUCCESS;
}

StatementResult prepare_statement(Statement& result, Token t)
{
    // TODO: implement statement syntax error
    std::string command = t.get_val();
    result.row_info.id = 0;
    result.row_info.name[0] = '\0';
    if (statement_type_map.find(command) == statement_type_map.end())
        return StatementResult::UNRECOGNIZED;
    result.type = statement_type_map[command];
    switch (result.type) {
    case (StatementType::INSERT):
        std::cin >> result.row_info.id >> result.row_info.name;
        return StatementResult::SUCCESS;
    case (StatementType::SHOW):
        return StatementResult::SUCCESS;
    case (StatementType::DELETE):
        // TODO
        return StatementResult::SUCCESS;
    }
    return StatementResult::UNRECOGNIZED;
}

void handle_input()
{
    TokenStream token_stream;
    Table table;
    table.db_open("tmp.txt");
    while (std::cin) {
        std::cout << prompt;
        Token t = token_stream.get();
        if (t.get_type() == TOKEN_UNSET)
            break;
        if (t.get_type() == meta_prefix) {
            switch (execute_meta_command(table, t)) {
            case MetaCommandResult::SUCCESS:
                continue;
            case MetaCommandResult::UNRECOGNIZED:
                std::cerr << "Command is not recognized.\n";
                continue;
            default:
                continue;
            }
        }

        Statement instruction;
        switch (prepare_statement(instruction, t)) {
        case (StatementResult::SUCCESS):
            break;
        case (StatementResult::SYNTAX_ERROR):
            std::cerr << "Syntax error for " + t.get_val() + " instruction.\n";
            continue;
        case (StatementResult::UNRECOGNIZED):
            std::cerr << "Unrecognized instruction: " + t.get_val() + ".\n";
            continue;
        }

        switch (dispatch_statement(table, instruction)) {
        case (ExecuteResult::SUCCESS):
            break;
        case (ExecuteResult::TABLE_FULL):
            std::cerr << "Error: Table full.\n";
            continue;
        case (ExecuteResult::FAIL):
            std::cerr << "Error: Execution failed.\n";
            continue;
        }
    }
    table.db_close();
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
