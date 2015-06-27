//
#include <Windows.h>
#include <string>
#include <unordered_set>

typedef void (*ServerProcessPointer)(unsigned int);

void serverProcessReplacement(unsigned int timeDelta);