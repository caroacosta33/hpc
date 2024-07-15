#ifndef SETHELPERS
#define SETHELPERS

#include <set>
#include <iterator>

double getNthValue(const std::set<double>& mySet, int n) {
    if (n < 0 || n >= mySet.size()) {
        throw std::out_of_range("Index out of range");
    }
    
    auto it = mySet.begin();
    std::advance(it, n);
    return *it;
}

#endif
