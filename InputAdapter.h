#ifndef __INPUT_ADAPTER_H__
#define __INPUT_ADAPTER_H__

#include "Joystick.h"
#include "EventHub.h"
#include "KeyManager.h"

namespace android {

class InputAdapter {
public:
	CREATE_FUNC(InputAdapter);
	int init();
	
	sp<Joystick> getJoystick();
	sp<KeyManager> getKeyManager();
	sp<EventHub> getEventHub();
	status_t start();
	status_t stop();
	void loopOnce();
	
private:
	sp<Joystick> mJoystick;
	sp<KeyManager> mKeyManager;
	sp<EventHub> mEventHub;
	sp<InputAdapterThread> mInputAdapterThread;
	Mutex mLock;
	
	static const int EVENT_BUFFER_SIZE = 256;
	RawEvent mEventBuffer[EVENT_BUFFER_SIZE];
};

class InputAdapterThread : public Thread {
public:
	InputAdapterThread(const sp<InputAdapter> &inputAdapter);
	virutal ~InputAdapterThread();
	
private:
	sp<InputAdpter> mAdapter;
	virtual bool threadLoop();
};

};

#endif