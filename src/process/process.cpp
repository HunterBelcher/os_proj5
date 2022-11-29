/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;


Process* Process::read_from_input(std::istream& in) {
  std::vector<Page*> pages = {};
  int total_bytes = 0;
  Page* page = nullptr;
  while(true){
   page = Page::read_from_input(in);
   if(page == nullptr)
   {
     break;
   }
   pages.push_back(page);
   total_bytes += page->size();
  }
  return new Process(total_bytes, pages);
}


size_t Process::size() const
{
    return this->num_bytes;
}


bool Process::is_valid_page(size_t index) const
{
    return index < this->pages.size();
}


size_t Process::get_rss() const
{
  size_t count = 0;
  for(int i = 0; i<this->page_table.rows.size(); i++){
    if(this->page_table.rows[i].present){
      count++;
    }
  }
  return count;
}


double Process::get_fault_percent() const
{
  if(this->memory_accesses == 0){
    return 0;
  }
  return ((double)this->page_faults / (double)this->memory_accesses) * 100;
}
