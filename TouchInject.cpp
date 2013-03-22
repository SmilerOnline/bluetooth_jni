#include "TouchInject.h"

namespace android {

int TouchInject::init() {
	eventHub = NULL;
	return 1;
}
	
int TouchInject::injectPointerSync(struct TouchEvent *touchEvent, int count) {
	if (eventHub == NULL) {
		//LOGE("[%s][%d] ==> eventHub == NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if (eventHub->injectTouchData(touchEvent, count) < 0) {
		//LOGE("[%s][%d] ==> eventHub->injectTouchData error", __FUNCTION__, __LINE__);
		return -1;
	}
	
	return 1;
}

}