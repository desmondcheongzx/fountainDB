#ifndef CURSOR_H
#define CURSOR_H

// Declare Table class from table.h
class Table;

class Cursor {
public:
    void goto_start(Table* t);
    void goto_end(Table* t);
    bool end() { return end_of_table; }
    void advance();
    void* value();
private:
    Table* table;
    uint_fast32_t row_num;
    bool end_of_table;
};

#endif
