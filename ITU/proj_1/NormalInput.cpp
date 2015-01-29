#include "NormalInput.h"
#include <time.h>

NormalInput::NormalInput()
{
}

float NormalInput::realTime(clock_t clockDiff)
{
	return ((float) clockDiff) / CLOCKS_PER_SEC;
}

QChar NormalInput::resolve(int index, QString charField)
{
	clock_t nowTime = clock();
	clock_t diff = nowTime - _lastTime;
	_lastTime = nowTime;

	if (index == _lastButton && realTime(diff) < QUICK_PRESS_INTEVAL) {
		/* Quick press, change last character */
		_lastResult = CHANGE_LAST;
		if (_lastPosition >= charField.length()) {
			_lastPosition = 0;
		}

		return charField[_lastPosition++];
	}

	_lastResult = APPEND_NEW;
	_lastButton = index;
	_lastPosition = 0;

	return charField[_lastPosition++];
}
