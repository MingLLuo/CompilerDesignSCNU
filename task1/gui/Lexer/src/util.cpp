#include "util.h"
static int fresh_counter = 0;
int fresh() { return ++fresh_counter; }
void flush() { fresh_counter = 0; }