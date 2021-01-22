#ifndef TABLE_H
#define TABLE_H

#define size_of_attribute(Struct, Attribute) sizeof(Struct::Attribute)
#define COLUMN_NAME_SIZE 32

// Each page stores 4 kB, similar to most virtual memory systems
const uint_fast32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100

typedef struct {
    uint_fast32_t id;
    char name[COLUMN_NAME_SIZE + 1];
} Row;

const uint_fast32_t ID_OFFSET = 0;
const uint_fast32_t ID_SIZE = size_of_attribute(Row, id);
const uint_fast32_t NAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint_fast32_t NAME_SIZE = size_of_attribute(Row, name);
const uint_fast32_t ROW_SIZE = ID_SIZE + NAME_SIZE;

const uint_fast32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint_fast32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void serialize_row(const Row& source, void* destination);
void deserialize_row(const void* source, Row& destination);
void print_row(const Row& row);

class Pager {
public:
    void set_file(const std::string& filename);
    char* get_page(uint_fast32_t page_num);
    void free_pager();
    uint_fast32_t get_rows() { return num_rows; }
    void inc_rows() { num_rows++; }
    void flush();
private:
    uint_fast32_t num_rows = 0;
    std::fstream fs;
    uint_fast32_t file_length{0};
    char* pages[TABLE_MAX_PAGES];
};

class Table {
public:
    void db_open(const std::string& filename);

    void free_table();
    void* row_slot(uint_fast32_t row_num);
    uint_fast32_t num_rows() { return pager.get_rows(); }
    void inc_rows() { pager.inc_rows(); }
    void db_close();
private:
    Pager pager;
};

#endif
