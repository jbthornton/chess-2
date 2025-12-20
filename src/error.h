#pragma once
#include <stdarg.h>

void error(const char* msg, ...);//test is prepended to msg before it is printed to stderr
void warning(const char* msg, ...);//prints message but doesn't exit the program 
