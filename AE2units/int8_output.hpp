#include <iostream>
#include <cstdint>

#ifndef INT8_OUTPUT_HPP
#define INT8_OUTPUT_HPP

std::ostream& operator<<(std::ostream& outputStream, const int8_t& num);

std::ostream& operator<<(std::ostream& outputStream, const uint8_t& num);

#endif
