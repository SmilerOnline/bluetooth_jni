#define LOG_TAG "INPUT_ADAPTER"

#include "InputAdapter.h"

namespace android {

//-----InputAdapter-----
int InputAdapter::init() {
	mJoystick = Joystick::create();
	mKeyManager = KeyManager:create();
	mEventHub = EventHub::create();
	
	mInputAdapterThread = new InputAdapterThread(this);
}

status_t InputAdapter::start() {
	int ret = mInputAdapterThread->run("InputAdapterThread", PRIORITY_URGENT_DISPLAY);
	if (ret) {
		 LOGE("[%s][%d] ==> Could not start InputReader thread due to error %d.", __FUNCTION__, __LINE__, ret);
		 mInputAdapterThread->requestExit();
	}
	
	return OK;
}

status_t InputAdapter::stop() {
    status_t result = mInputAdapterThread->requestExitAndWait();
    if (result) {
        LOGE("[%s][%d] ==> Could not stop InputReader thread due to error %d.", __FUNCTION__, __LINE__, result);
    }

    return OK;
}

sp<Joystick> InputAdapter::getJoystick() {
	return mJoystick;
}

sp<KeyManager> InputAdapter::getKeyManager() {
	return mKeyManager;
}

sp<EventHub> InputAdapter::getEventHub() {
	return mEventHub;
}

void InputAdapter::loopOnce() {
	AutoMutext _l(mLock);
	memset(mEventBuffer, 0, EVENT_BUFFER_SIZE);
	mEventHub->getEvents(0, mEventBuffer, EVENT_BUFFER_SIZE);
	processRawEventLocked(mEventBuffer);
}

void InputAdapter::processRawEventLocked(const RawEvent *eventBuffer) {
	for (size_t i = 0; i < eventBuffer->count; i ++) {
		switch (eventBuffer->type) {
		case EV_KEY:
			mKeyManager->processKeys(eventBuffer);
			break;
		case EV_ABS:
			break;
		}
		eventBuffer ++;
	}
}

//---- InputAdapterThread-----
InputAdapterThread::InputAdapter(const sp<InputAdapter> &adapter):
	Thread(true), mAdapter(adapter) {
	
}

InputAdapterThread::~InputAdapterThread() {
	
}

bool InputAdapterThread::threadLoop() {
	mAdapter->loopOnce();
	return true;
}

};