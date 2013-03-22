#ifndef __CALLBACK_INTERFACE_H__
#define __CALLBACK_INTERFACE_H__

#include "EventHub.h"

namespace android {

class CallBackInterface {
public:
	virtual int keyProcess(RawEvent *rawEvent) {};
	virtual int joystickProcess(RawEvent *rawEvent) {};
};

};

#endif