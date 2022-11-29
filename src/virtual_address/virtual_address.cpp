/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "virtual_address/virtual_address.h"

using namespace std;

VirtualAddress VirtualAddress::from_string(int process_id, string address) {
 return VirtualAddress(process_id, stoi(address.substr(0, PAGE_BITS), 0, 2), stoi(address.substr(PAGE_BITS, OFFSET_BITS), 0, 2));
}


string VirtualAddress::to_string() const {
  return std::bitset<10>(this->page).to_string() + std::bitset<6>(this->offset).to_string();
}


ostream& operator <<(ostream& out, const VirtualAddress& address) {
  out << "PID " << address.process_id << " @ " << address.to_string() << " [page: " << address.page << "; offset: " << address.offset << "]";
  return out;
}
