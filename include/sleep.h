#ifndef SLEEP_H_
#define SLEEP_H_

#ifdef _WINDOWS
	#include <Windows.h>
#else
	#include <unistd.h>
	#define Sleep(x) usleep((x) * 1000)
#endif

#endif
