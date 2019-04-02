#include <Arduino.h>
#include <limits.h>

#define UL_SUB(a, b) (((b) > (a)) ? ((ULONG_MAX - (b)) + (a)) : ((a) - (b)))
#define UL_LESS(a, b) ((UL_SUB(b, a)) < (ULONG_MAX >> 1))

class Stepper {
	int stepPin;
	int homePin;

	unsigned long halfStepTime;
	int stepPinState;
	unsigned long nextStepTime;

public:
	Stepper(int stepPin, int homePin, unsigned long stepTime = 1000ul)
		:stepPin(stepPin), homePin(homePin)
	{
		halfStepTime = stepTime >> 1;
		stepPinState = LOW;
		nextStepTime = micros();
	}

	void setup()
	{
		pinMode(stepPin, OUTPUT);
		pinMode(homePin, INPUT);

		digitalWrite(stepPin, stepPinState);
	}

	void loop()
	{
		unsigned long now = micros();

		if (UL_LESS(nextStepTime, now)) {
			stepPinState = stepPinState == HIGH ? LOW : HIGH;
			digitalWrite(stepPin, stepPinState);
			nextStepTime = now + halfStepTime;
		}
	}
};
