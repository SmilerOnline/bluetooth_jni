#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "CallBackInterface.h"

namespace android {

class Joystick {
public:
	CREATE_FUNC(Joystick);
	int init() {
		mCallBackInterface = NULL;
		return 1;
	}
	
	void registerCallBackInterface(CallBackInterface *callBackInterface) {
		mCallBackInterface = callBackInterface;
		LOGE("[%s][%d] ==> register callbackinterface", __FUNCTION__, __LINE__);
	}
	
	int joystickProcess(RawEvent *rawEvent) {
		if (mCallBackInterface == NULL) {
			LOGE("[%s][%d] ==> mCallBackInterface is NULL", __FUNCTION__, __LINE__);
			return -1;
		}
		
		mCallBackInterface->joystickProcess(rawEvent);
		return 1;
	}
	
private:
	CallBackInterface *mCallBackInterface;
	
};

};

#endif