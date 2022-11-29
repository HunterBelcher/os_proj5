/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "physical_address/physical_address.h"

using namespace std;

string PhysicalAddress::to_string() const {
    return std::bitset<10>(this->frame).to_string() + std::bitset<6>(this->offset).to_string();
}


ostream& operator <<(ostream& out, const PhysicalAddress& address) {
    out << address.to_string() << " [frame: " << address.frame << "; offset: " << address.offset << "]";
    return out;
}
