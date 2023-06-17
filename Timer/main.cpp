#include <iostream>
#include "Timer.h"

Timer wTimer(10);

int main() {
	while (true)
	{
		wTimer.StartTiming("我靠");
		wTimer.StartEnd();
		wTimer.RefreshTiming();
	}
	return 0;
}
