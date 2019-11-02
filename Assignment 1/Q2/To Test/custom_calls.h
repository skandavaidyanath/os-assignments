#include <unistd.h>

int myfork()
{
	return syscall(355);
}