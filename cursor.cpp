#include "table.h"
#include "cursor.h"

void Cursor::goto_start(Table* t)
{
    table = t;
    row_num = 0;
    end_of_table = (table->num_rows() == 0);
}

void Cursor::goto_end(Table* t)
{
    table = t;
    row_num = table->num_rows();
    end_of_table = true;
}

void Cursor::advance()
{
    row_num++;
    if (row_num >= table->num_rows())
        end_of_table = true;
}

void* Cursor::value()
{
    uint_fast32_t page_num = row_num / ROWS_PER_PAGE;
    char* page = table->get_page(page_num);

    uint_fast32_t row_offset = row_num % ROWS_PER_PAGE;
    uint_fast32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}
