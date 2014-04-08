#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>
#include <MySnmp/SafeType.h>

#ifdef __GNUC__
#include <signal.h>
#endif

namespace mysnmp {
	typedef void * (*ThreadFunc)(void *);

	enum ThreadStatus {
		ready, running, terminated, joined, error,
	};

	class Thread {
	private:
		static SafeInteger nextInnerId;

		int innerId;
		pthread_t tid;
		ThreadFunc func;
		ThreadStatus status;
		const char * errMsg;

	public:
		Thread(ThreadFunc threadFunc) : func(threadFunc), status(ThreadStatus::ready), errMsg(NULL) {
			this->innerId = nextInnerId.GetAndInc();
		};

		~Thread() {}

		inline int GetInnerId() {
			return innerId;
		}

		bool Run(void * data);
		bool Join(void ** retval);
		int Cancel();
		const pthread_t * GetTid();
		ThreadStatus GetStatus();
		const char * GetErrMsg();
	};
}


#endif