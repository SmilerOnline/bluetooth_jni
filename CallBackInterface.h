#ifndef __CALLBACK_INTERFACE_H__
#define __CALLBACK_INTERFACE_H__

#include "EventHub.h"

namespace android {

class CallBackInterface  : public RefBase  {
public:
	int keyProcess(const RawEvent *rawEvent) { return 1; };
	int joystickProcess(const RawEvent *rawEvent) { return 1; };
};

};

#endif