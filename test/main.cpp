#define LOG_TAG "event_hub_test"

#include <stdio.h>
#include <stdlib.h>
#include "../EventHub.h"
#include <pthread.h>
#include "../TouchInject.h"
 
using namespace android;

int pointer = 0;

void testTapPointer(TouchInject *touchInject) {
	struct TouchEvent event;
	memset(&event, 0, sizeof(struct TouchEvent));
	LOGE("testTouchOnePointer");
	event.x = 500;
	event.y = 400;
	event.width_major = 1;
	event.touch_major = 0xc8;
	event.btn_touch = 1;
	event.pointer = 0;
	event.pressure = 0;
	event.touchtype = TOUCH_TYPE_SINGLE_POINTER;
	int ret = touchInject->injectPointerSync(&event, 1);
	if (ret < 0) {
		printf("down inject pointer sync error");
	}
	
	usleep(1000);
	
	memset(&event, 0, sizeof(struct TouchEvent));
	event.x = 0;
	event.y = 0;
	event.pointer = 0;
	event.pressure = 0xffffffff;
	event.touchtype = TOUCH_TYPE_RELEASE;

	ret = touchInject->injectPointerSync(&event, 1);
	if (ret < 0) {
		printf("up inject pointer sync error");
	}
}

void testTouchOnePointer(TouchInject *touchInject) {
	struct TouchEvent event;
	memset(&event, 0, sizeof(struct TouchEvent));
	LOGE("testTouchOnePointer");
	for (int i = 1280; i >= 0; i --) {
		event.x = 700;
		event.y = i;
		event.width_major = 1;
		event.touch_major = 0xc8;
		event.btn_touch = 1;
		event.pointer = 0;
		event.pressure = 0;
		event.touchtype = TOUCH_TYPE_SINGLE_POINTER;
		int ret = touchInject->injectPointerSync(&event, 1);
		if (ret < 0) {
			printf("down inject pointer sync error");
		}
		
		usleep(1000);
	}
	
	memset(&event, 0, sizeof(struct TouchEvent));
	event.x = 0;
	event.y = 0;
	event.pointer = 0;
	event.pressure = 0xffffffff;
	event.touchtype = TOUCH_TYPE_RELEASE;
	int ret = touchInject->injectPointerSync(&event, 1);
	if (ret < 0) {
		printf("up inject pointer sync error");
	}
}

void testTouchTwoPointer(TouchInject *touchInject) {
	struct TouchEvent event[2];
	memset(event, 0, sizeof(event));
	LOGE("testTouchTwoPointer");
	for (int i = 0; i <= 1280; i ++) {
		event[0].x = i;
		event[0].y = 0;
		event[0].width_major = 1;
		event[0].touch_major = 0xc8;
		event[0].touchtype = TOUCH_TYPE_MT_POINTER;
		event[0].pointer = 0;
		event[0].pressure = 0;
		event[0].btn_touch = 1;
		
		event[1].x = i;
		event[1].y = 100;
		event[1].width_major = 1;
		event[1].touch_major = 0xc8;
		event[1].btn_touch = 1;
		event[1].pressure = 1;
		event[1].touchtype = TOUCH_TYPE_MT_POINTER;
		event[1].pointer = 1;
		int ret = touchInject->injectPointerSync(event, sizeof(event) / sizeof(struct TouchEvent));
		if (ret < 0) {
			printf("down inject pointer sync error");
		}
		
		usleep(1000);
	}
	
	memset(event, 0, sizeof(event));
	event[0].pointer = 0;
	event[0].pressure = 0xffffffff;
	event[0].touchtype = TOUCH_TYPE_RELEASE;
	event[1].touchtype = TOUCH_TYPE_RELEASE;
	event[1].pointer = 1;
	event[1].pressure = 0xffffffff;
	int ret = touchInject->injectPointerSync(event, sizeof(event)/sizeof(struct TouchEvent));
	if (ret < 0) {
		printf("up inject pointer sync error");
	}
}

void testTouchTwoPointerTap(TouchInject *touchInject) {
	struct TouchEvent event[2];
	memset(event, 0, sizeof(event));
	LOGE("testTouchTwoPointer");
	event[0].x = 700;
	event[0].y = 400;
	event[0].touchtype = TOUCH_TYPE_MT_POINTER;
	event[0].pointer = 0;
	event[0].pressure = 0;
	event[0].width_major = 1;
	event[0].touch_major = 0xc8;
	event[0].btn_touch = 1;
		
	event[1].x = 500;
	event[1].y = 200;
	event[1].touchtype = TOUCH_TYPE_MT_POINTER;
	event[1].pointer = 1;
	event[1].pressure = 1;
	event[1].width_major = 1;
	event[1].touch_major = 0xc8;
	event[1].btn_touch = 1;
	int ret = touchInject->injectPointerSync(event, sizeof(event) / sizeof(struct TouchEvent));
	if (ret < 0) {
		printf("down inject pointer sync error");
	}
	
	usleep(1000);
	
	memset(event, 0, sizeof(event));
	event[0].pointer = 0;
	event[0].pressure = 0xffffffff;
	event[0].touchtype = TOUCH_TYPE_RELEASE;
	event[1].touchtype = TOUCH_TYPE_RELEASE;
	event[1].pointer = 1;
	event[1].pressure = 0xffffffff;
	ret = touchInject->injectPointerSync(event, sizeof(event)/sizeof(struct TouchEvent));
	if (ret < 0) {
		printf("up inject pointer sync error");
	}
}



void *loop_thread(void *data) {
	TouchInject *touchInject = (TouchInject*)data;
	if (touchInject == NULL) {
		printf("touchInject is null thread exit");
		return NULL;
	}
	
	EventHub *ehub = touchInject->getEventHub();

	RawEvent buffer[512];
	memset(buffer, 0, sizeof(buffer));
	
	if (ehub != NULL) {
		while (1) {
			ehub->getEvents(0, buffer, sizeof(buffer));
			usleep(1000);
		}
	} else {
		LOGE("ehub is null");
	}

	return NULL;
}

void *loop_touch_inject_test_thread(void *data) {
	TouchInject *touchInject = (TouchInject*)data;
		if (touchInject == NULL) {
			printf("touchInject is null thread exit");
			return NULL;
		}
		
		EventHub *ehub = touchInject->getEventHub();
		if (ehub != NULL) {
		//	while (1) {
				if (pointer == 1)
					testTouchOnePointer(touchInject);
				else if (pointer == 2) 
					testTouchTwoPointer(touchInject);
				else if (pointer == 3)
					testTapPointer(touchInject);
				else if (pointer == 4)
					testTouchTwoPointerTap(touchInject);
				usleep(2000);
			}
		//}
		return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t tid;
	pthread_t tid1;

	LOGE("argc = %d", argc);
	if (argc == 2) {
		if (strcmp(argv[1], "1") == 0) pointer = 1;
		else if (strcmp(argv[1], "2") == 0) pointer = 2;
		else if (strcmp(argv[1], "3") == 0) pointer = 3;
		else if (strcmp(argv[1], "4") == 0) pointer = 4;
	}
	LOGE("pointer = %d", pointer);
	EventHub *ehub = EventHub::create();
	ehub->scanInput();
	TouchInject *touchInject = TouchInject::create();
	if (touchInject != NULL) {
		touchInject->setEventHub(ehub);
	} else {
		LOGE("touchinject is null");
		exit(1);
	}
	int err;
	if (ehub != NULL) {
#if 0
		err = pthread_create(&tid, NULL,loop_thread, touchInject);
		if (err) {
			printf("create thread error\n");
			exit(1);
		}
#endif	
		err = pthread_create(&tid1, NULL,loop_touch_inject_test_thread, touchInject);
		if (err) {
			printf("create thread error\n");
			exit(1);
		}
		//pthread_join(tid, NULL);
		pthread_join(tid1, NULL);
	} else {
		LOGE("ehub = null");
	}

	return 0;
}
