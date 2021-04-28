#include <limits.h>

#define UL_SUB(a, b) (((b) > (a)) ? ((ULONG_MAX - (b)) + (a)) : ((a) - (b)))
#define UL_LESS(a, b) ((UL_SUB(b, a)) < (ULONG_MAX >> 1))

class Stepper {
	int stepPin;
	int homePin;
	unsigned int stepsPerRevolution;
	unsigned long halfStepTime;

	int targetPosition;

	int stepPinState;
	unsigned long nextStepTime;
	bool wasHome;
	int stepsFromHome;

public:
	Stepper(int stepPin, int homePin, unsigned int stepsPerRevolution, unsigned long stepTime = 1000ul)
		:stepPin(stepPin), homePin(homePin), stepsPerRevolution(stepsPerRevolution)
	{
		halfStepTime = stepTime >> 1;
		targetPosition = 0;

		stepPinState = LOW;
		nextStepTime = micros();
		wasHome = true;
		stepsFromHome = -1;
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

		bool doStep = (targetPosition == -1) || (targetPosition != stepsFromHome);
		if (doStep && UL_LESS(nextStepTime, now)) {
			step();
			nextStepTime = now + halfStepTime;
			checkHome();
		}
	}

	void stop()
	{
		targetPosition = stepsFromHome;
	}

	void spin()
	{
		targetPosition = -1;
	}

	void step(int num)
	{
		targetPosition = normalizePosition(stepsFromHome + num);
	}

	void goTo(int position)
	{
		targetPosition = normalizePosition(position);
	}

	void home()
	{
		goTo(0);
	}

	int currentPosition() { return stepsFromHome; }

	bool isHome() { return stepsFromHome == 0; }

private:
	void step()
	{
		if (stepPinState == LOW) {
			stepPinState = HIGH;
			if (stepsFromHome != -1) {
				stepsFromHome++;
			}
		} else {
			stepPinState = LOW;
		}
		digitalWrite(stepPin, stepPinState);
	}

	void checkHome()
	{
		bool home = (digitalRead(homePin) == HIGH);
		if (!wasHome && home) {
			stepsFromHome = 0;
		}
		wasHome = home;
	}

	int normalizePosition(int position)
	{
		while (position < 0) position += stepsPerRevolution;
		return position % stepsPerRevolution;
	}
};
