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

char* make_page()
{
    char* new_page = new char[PAGE_SIZE];
    return new_page;
}

void Pager::set_file(const std::string& filename)
{
    fs = std::fstream{
        filename,
        std::ios_base::in|
        std::ios_base::out|
        std::ios_base::binary|
        std::ios_base::ate};
    if (!fs)
        error("Can't open input file " + filename);
    file_length = fs.tellg();
    
    num_rows = file_length / ROW_SIZE;

    for (uint_fast32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pages[i] = NULL;
    }
}

char* Pager::get_page(uint_fast32_t page_num)
{
    if (pages[page_num] != NULL)
        return pages[page_num];

    char* page = make_page();
    uint_fast32_t num_pages = file_length / PAGE_SIZE;
    if (file_length % PAGE_SIZE)
        ++num_pages;

    if (page_num <= num_pages) {
        fs.seekg(page_num * PAGE_SIZE);
        fs.read(page, PAGE_SIZE);
        // check for reading error?
    }

    return pages[page_num] = page;
}

void Pager::free_pager()
{
    uint_fast32_t num_pages = num_rows / ROWS_PER_PAGE;
    for (uint_fast32_t i = 0; i <= num_pages; i++) {
        if (pages[i] == NULL)
            continue;
        delete[] pages[i];
        pages[i] = NULL;
    }
}

void Pager::flush()
{
    uint_fast32_t num_full_pages = num_rows / ROWS_PER_PAGE;
    for (uint_fast32_t i = 0; i < num_full_pages; i++) {
        if (pages[i] == NULL)
            continue;
        fs.seekp(i * PAGE_SIZE);
        fs.write(pages[i], PAGE_SIZE);
    }
    uint_fast32_t num_additional_rows  = num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint_fast32_t partial_page_num = num_full_pages;
        if (pages[partial_page_num] != NULL) {
            fs.seekp(partial_page_num * PAGE_SIZE);
            fs.write(pages[partial_page_num], num_additional_rows * ROW_SIZE);
        }
    }

}

void Table::db_open(const std::string& filename)
{
    pager.set_file(filename);
}

void* Table::row_slot(uint_fast32_t row_num)
{
    uint_fast32_t page_num = row_num / ROWS_PER_PAGE;
    char* page = pager.get_page(page_num);

    uint_fast32_t row_offset = row_num % ROWS_PER_PAGE;
    uint_fast32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void Table::db_close()
{
    pager.flush();
    pager.free_pager();
}
