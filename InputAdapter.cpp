#define LOG_TAG "INPUT_ADAPTER"

#include "InputAdapter.h"
#include "debug.h"

namespace android {

//-----InputAdapter-----

void InputAdapter::release() {
	mEventHub->release();
}

int InputAdapter::init() {
	mJoystick = Joystick::create();
	mKeyManager = KeyManager::create();
	mEventHub = EventHub::create();
	
	mInputAdapterThread = new InputAdapterThread(this);
	mInputAdapterNotifierThread = new InputAdapterNotifierThread(this);
	
	return 1;
}

status_t InputAdapter::start() {
	mEventHub->scanInput();
	int ret = mInputAdapterThread->run("InputAdapterThread", PRIORITY_URGENT_DISPLAY);
	if (ret) {
		 LOGE("[%s][%d] ==> Could not start InputAdapterThread thread due to error %d.", __FUNCTION__, __LINE__, ret);
		 mInputAdapterThread->requestExit();
		 return -1;
	}
	ret = mInputAdapterNotifierThread->run("InputAdapterNotifierThread", PRIORITY_URGENT_DISPLAY);
	if (ret) {
		 LOGE("[%s][%d] ==> Could not start InputAdapterNotifierThread thread due to error %d.", __FUNCTION__, __LINE__, ret);
		 mInputAdapterNotifierThread->requestExit();
		 return -1;
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
	AutoMutex _l(mLock);
	memset(mEventBuffer, 0, EVENT_BUFFER_SIZE);
	mEventHub->getEvents(0, mEventBuffer, EVENT_BUFFER_SIZE);
	processRawEventLocked(mEventBuffer);
}

void InputAdapter::dumpRawEvent(const RawEvent *event) {
	LOGE("[%s][%d] ==> event.type = 0x%02x, event.scancode = 0x%02x event.value = 0x%02x event.deviceid = %d",
			__FUNCTION__, __LINE__, event->type, event->scanCode, event->value, event->deviceId);
}

void InputAdapter::processRawEventLocked(const RawEvent *eventBuffer) {
	for (size_t i = 0; i < eventBuffer->count; i ++) {
#if DEBUG_SWITCH
		dumpRawEvent(eventBuffer);
#endif
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
InputAdapter::InputAdapterThread::InputAdapterThread(sp<InputAdapter> adapter):
	Thread(true), mInputAdapter(adapter) {
	
}

InputAdapter::InputAdapterThread::~InputAdapterThread() {
	
}

bool InputAdapter::InputAdapterThread::threadLoop() {
	mInputAdapter->loopOnce();
	return true;
}

//---- InputAdapterNotifierThread-----
InputAdapter::InputAdapterNotifierThread::InputAdapterNotifierThread(sp<InputAdapter> adapter):
	Thread(true), mInputAdapter(adapter) {
	
}

InputAdapter::InputAdapterNotifierThread::~InputAdapterNotifierThread() {
	
}

bool InputAdapter::InputAdapterNotifierThread::threadLoop() {
	LOGE("[%s][%d] ==> readNotifyLocked", __FUNCTION__, __LINE__);
	mInputAdapter->getEventHub()->readNotifyLocked();
	return true;
}

};