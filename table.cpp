#include "utils.h"
#include "table.h"

void serialize_row(const Row& source, void* destination)
{
    memcpy((char*) destination + ID_OFFSET, &(source.id), ID_SIZE);
    strncpy((char*) destination + NAME_OFFSET, source.name, NAME_SIZE);
}

void deserialize_row(const void* source, Row& destination)
{
    memcpy(&(destination.id), source, ID_SIZE);
    strncpy(destination.name, (char*) source + NAME_OFFSET, NAME_SIZE);
}

void print_row(const Row& row)
{
    std::cout << (uint_fast32_t) row.id << " ";
    std::cout << (std::string) row.name << std::endl;
}

Table::Table() {
    pages.reserve(TABLE_MAX_PAGES);
}

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

void* Table::row_slot(uint_fast32_t row_num)
{
    uint_fast32_t page_num = row_num / ROWS_PER_PAGE;
    char* page;
    if (page_num >= pages.size()) {
        pages.push_back(make_page());
        page = pages.back();
    } else {
        page = pages.at(page_num);
    }
    uint_fast32_t row_offset = row_num % ROWS_PER_PAGE;
    uint_fast32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}
