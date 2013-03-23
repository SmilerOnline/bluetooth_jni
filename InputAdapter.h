#ifndef __INPUT_ADAPTER_H__
#define __INPUT_ADAPTER_H__

#include "Joystick.h"
#include "EventHub.h"
#include "KeyManager.h"

namespace android {

class InputAdapter : public virtual RefBase {
public:
	CREATE_FUNC(InputAdapter);
	int init();
	void release();
	
	sp<Joystick> getJoystick();
	sp<KeyManager> getKeyManager();
	sp<EventHub> getEventHub();
	status_t start();
	status_t stop();
	void loopOnce();
	void processRawEventLocked(const RawEvent *eventBuffer);
	void dumpRawEvent(const RawEvent *event);
	
	class InputAdapterThread : public Thread {
	public:
		InputAdapterThread(sp<InputAdapter> adapter);
		virtual ~InputAdapterThread();
		
	private:
		virtual bool threadLoop();
		sp<InputAdapter> mInputAdapter;
	};
	
	class InputAdapterNotifierThread : public Thread {
		public:
			InputAdapterNotifierThread(sp<InputAdapter> adapter);
			virtual ~InputAdapterNotifierThread();
			
		private:
			virtual bool threadLoop();
			sp<InputAdapter> mInputAdapter;
		};
	
private:
	sp<Joystick> mJoystick;
	sp<KeyManager> mKeyManager;
	sp<EventHub> mEventHub;
	sp<InputAdapterThread> mInputAdapterThread;
	sp<InputAdapterNotifierThread> mInputAdapterNotifierThread;
	Mutex mLock;
	
	static const int EVENT_BUFFER_SIZE = 256;
	RawEvent mEventBuffer[EVENT_BUFFER_SIZE];
};

};

#endif