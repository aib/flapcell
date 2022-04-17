#ifndef FLAPDRIVER_H__
#define FLAPDRIVER_H__

#include "StreamDriver.h"

#include "FStepper.h"

class FlapDriver: public StreamDriver {
public:
	const FStepper* steppers;
	const size_t stepperCount;

	FlapDriver(FStepper* steppers, size_t stepperCount, Stream& stream)
		:StreamDriver(stream), steppers(steppers), stepperCount(stepperCount)
	{
	}

	void setup()
	{
		addCommand('H', home, "Home all cells");
		addCommand('p', printpos, "Print cell positions");

		StreamDriver::setup();
	}

private:
	static void home(FlapDriver* driver, const char* cmd, size_t cmdLen)
	{
		for (size_t i = 0; i < driver->stepperCount; i++) {
			driver->steppers[i].home();
		}
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
