#include <iostream>
#include <cstdint>

#include "int8_output.hpp"

std::ostream& operator<<(std::ostream& outputStream, const int8_t& num) {
	outputStream << static_cast<int>(num);
	return outputStream;
}

std::ostream& operator<<(std::ostream& outputStream, const uint8_t& num) {
	outputStream << static_cast<unsigned int>(num);
	return outputStream;
}
