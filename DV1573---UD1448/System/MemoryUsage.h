#ifndef _MEMORY_USAGE_H
#define _MEMORY_USAGE_H
#include <Pch/Pch.h>

class MemoryUsage {
public:
	void printVramUsage();
	void printRamUsage();
	void printBoth(std::string string);

};

#endif
