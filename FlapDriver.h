#ifndef FLAPDRIVER_H__
#define FLAPDRIVER_H__

#include "StreamDriver.h"

#include <string.h>

#include "FStepper.h"

class FlapDriver: public StreamDriver {
public:
	const FStepper* steppers;
	const size_t stepperCount;
	const char* alphabet;

private:
	size_t alphabetSize;


public:
	FlapDriver(FStepper* steppers, size_t stepperCount, Stream& stream, const char* alphabet)
		:StreamDriver(stream), steppers(steppers), stepperCount(stepperCount), alphabet(alphabet)
	{
		alphabetSize = strlen(alphabet);
	}

	void setup()
	{
		addCommand('A', alpha, "Set cells using alphabet");
		addCommand('H', home, "Home all cells");
		addCommand('a', printalpha, "Print cell alphabet");
		addCommand('p', printpos, "Print cell positions");

		StreamDriver::setup();
	}

private:
	static void alpha(FlapDriver* driver, const char* cmd, size_t cmdLen)
	{
		for (size_t s = 0; s < driver->stepperCount && s < cmdLen; s++) {
			size_t posPerFlap = driver->steppers[s].stepsPerRevolution / driver->alphabetSize;
			for (size_t a = 0; a < driver->alphabetSize; a++) {
				if (driver->alphabet[a] == cmd[s]) {
					driver->steppers[s].goTo(a * posPerFlap);
					break;
				}
			}
		}
	}

	static void home(FlapDriver* driver, const char* cmd, size_t cmdLen)
	{
		for (size_t i = 0; i < driver->stepperCount; i++) {
			driver->steppers[i].home();
		}
	}

	static void printalpha(FlapDriver* driver, const char* cmd, size_t cmdLen)
	{
		driver->stream.print("a ");
		driver->stream.print(driver->alphabetSize);
		driver->stream.print(" [");
		for (size_t i = 0; i < driver->alphabetSize; i++) {
			driver->stream.print(driver->alphabet[i]);
		}
		driver->stream.println("]");
	}

	static void printpos(FlapDriver* driver, const char* cmd, size_t cmdLen)
	{
		driver->stream.print("p");
		for (size_t i = 0; i < driver->stepperCount; i++) {
			driver->stream.print(" ");

			driver->stream.print(driver->steppers[i].currentPosition());
			if (driver->steppers[i].isHome()) {
				driver->stream.print("H");
			}
		}
		driver->stream.println();
	}
};

#endif
