#ifndef NORMALINPUT_H
#define NORMALINPUT_H

#include <QString>
#include <time.h>

#define QUICK_PRESS_INTEVAL 1.0

enum RESULT_TYPE {
    APPEND_NEW,
    CHANGE_LAST,
};

class NormalInput
{
public:
    NormalInput();


    QChar resolve(int index, QString charField);

    void reset() { _lastTime = 0; _lastButton = -1; }

    int lastResult() { return _lastResult; }
private:

    clock_t _lastTime{};
    int _lastButton{-1};
    int _lastPosition{0};
    int _lastResult{APPEND_NEW};
    float realTime(clock_t clockDiff);
};

#endif // NORMALINPUT_H
