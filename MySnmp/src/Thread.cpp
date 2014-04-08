#include <MySnmp/debug.h>

#include <MySnmp/Thread.h>
#include <errno.h>
#include <string.h>

using namespace mysnmp;

SafeInteger Thread::nextInnerId(new SpinLock(), 0);

bool Thread::Run(void * data) {
	int ret = pthread_create(&this->tid, NULL, this->func, data);
	if (ret != 0) {
		this->status = ThreadStatus::error;
		this->errMsg = strerror(errno);
		return false;
	}
	this->status = ThreadStatus::running;
	return true;
}

bool Thread::Join(void ** retval) {
	ThreadStatus status = GetStatus();
	if (status != ThreadStatus::running && status != ThreadStatus::joined) {
		this->errMsg = "Thread is not running";
		return false;
	}
	if (status == ThreadStatus::joined) {
		this->errMsg = "Thread already joined";
		return false;
	}
	this->status = ThreadStatus::joined;
	int ret = pthread_join(this->tid, retval);
	if (ret != 0) {
		this->status = ThreadStatus::error;
		this->errMsg = strerror(errno);
		return false;
	}
	return true;
}

int Thread::Cancel() {
	return pthread_cancel(this->tid);
}

const pthread_t * Thread::GetTid() {
	if (GetStatus() != ThreadStatus::running)
		return NULL;
	return &this->tid;
}

ThreadStatus Thread::GetStatus() {
	if (this->status == ThreadStatus::running ||
		this->status == ThreadStatus::joined) {
		int kill_rc = pthread_kill(this->tid, 0);
		if (kill_rc == ESRCH)
			this->status = ThreadStatus::terminated;
		else if (kill_rc == EINVAL)
			this->status = ThreadStatus::error;
	}
	return this->status;
}


//#include <iostream>
//void * test(void * data) {
//	for (int i = 0; i < 10000; i++)
//		std::cout << "1";
//	return NULL;
//}
//
//void main() {
//	Thread thread(test);
//	thread.Run(NULL);
//	const pthread_t * tid = thread.GetTid();
//	ThreadStatus status1 = thread.GetStatus();
//	thread.Join(NULL);
//	ThreadStatus status = thread.GetStatus();
//}