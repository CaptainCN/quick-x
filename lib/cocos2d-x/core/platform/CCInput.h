
#pragma once

#include "engine/CCSubSystem.h"
#include "CCKeyCode.h"

NS_CC_BEGIN;


class Input : public SubSystem
{
	CCOBJECT(Input, SubSystem)

public:

	void setAccelerationEnable(bool enable);
	void setAccelerationInterval(float interval);

public:
	void onKeypadBack();
	void onKeypadMenu();

	void onEnterForeground();
	void onEnterBackground();

	void onAcceleration(float x, float y, float z, float timeStamp);

	void update();

private:
	void handleEvent(void* evt);
};

NS_CC_END;