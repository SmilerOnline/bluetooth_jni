#ifndef __TOUCH_INJECT_H__
#define __TOUCH_INJECT_H__

#include "EventHub.h"

namespace android {

class TouchInject {
public:
	CREATE_FUNC(TouchInject);
	int init();
	
	int injectPointerSync(struct TouchEvent *touchEvent, int count);
	void setEventHub(EventHub *hub) { eventHub = hub; }
	EventHub *getEventHub() { return eventHub; }
private:
	EventHub *eventHub;
};

};

#endif