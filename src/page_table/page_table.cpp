/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "page_table/page_table.h"

using namespace std;


size_t PageTable::get_present_page_count() const {
  size_t count = 0;
  for(int i = 0; i<this->rows.size();i++){
    if(this->rows[i].present){
      count++;
    }
  }
  return count;
}


size_t PageTable::get_oldest_page() const {
  int min_loaded_at = 0;
  int min_loaded_val = 0;
  for(int i = 0; i<this->rows.size();i++){
    if(this->rows[i].present){
      if(min_loaded_val == 0) {
        min_loaded_at = i;
        min_loaded_val = this->rows[i].loaded_at;
      }
      if(this->rows[i].loaded_at < min_loaded_val){
        min_loaded_at = i;
        min_loaded_val = this->rows[i].loaded_at;
      }
    }
  }
  return min_loaded_at;
}


size_t PageTable::get_least_recently_used_page() const {
  int min_loaded_at = 0;
  int min_loaded_val = 0;
  for(int i = 0; i<this->rows.size();i++){
    if(this->rows[i].present){
      if(min_loaded_val == 0) {
        min_loaded_at = i;
        min_loaded_val = this->rows[i].last_accessed_at;
      }
      if(this->rows[i].last_accessed_at < min_loaded_val){
        min_loaded_at = i;
        min_loaded_val = this->rows[i].last_accessed_at;
      }
    }
  }
  return min_loaded_at;
}