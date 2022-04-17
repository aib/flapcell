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
		StreamDriver::setup();
	}
};

#endif
