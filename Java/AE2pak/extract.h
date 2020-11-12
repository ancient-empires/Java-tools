#ifndef EXTRACT_H
#define EXTRACT_H

// Extract .pak file to extract directory.
// The extract directory must exist beforehand, or the program will NOT work.
// Write the .log file containing the list of all extracted files in current working directory.
void extract(const char* pakFile, const char* extractDir);

#endif
