#include <iostream>
#include <cstdint>

#ifndef INT8_OUTPUT_HPP
#define INT8_OUTPUT_HPP

// This header file overloads the << operator.
// Hence the int8_t and uint8_t integers are printed as digits.

std::ostream& operator<<(std::ostream& outputStream, const int8_t& num);

std::ostream& operator<<(std::ostream& outputStream, const uint8_t& num);

#endif
