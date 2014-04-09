#ifndef __SEMAPHORE_H
#define __SEMAHPORE_H

#include <semaphore.h>

namespace mysnmp {
	class Semaphore {
	private:
		sem_t sem;
	public:
		Semaphore(int initialValue) {
			sem_init(&this->sem, 0, initialValue);
		}

		~Semaphore() {
			sem_destroy(&this->sem);
		}

		int GetValue() {
			int retval;
			int err;
			err = sem_getvalue(&this->sem, &retval);
			if (err != 0)
				return -0x7FFFFFFF;
			return retval;
		}

		int Post() {
			return sem_post(&this->sem);
		}

#ifdef _WIN32
		int Post(int num) {
			return sem_post_multiple(&this->sem, num);
		}
#endif
		int Wait() {
			return sem_wait(&this->sem);
		}
	};
}

#endif