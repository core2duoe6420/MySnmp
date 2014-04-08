#ifndef __LOCK_H
#define __LOCK_H

#include <pthread.h>

namespace mysnmp {
	class Lock {
	public:
		virtual ~Lock() {}

		virtual int Enter(int param) = 0;
		virtual int TryEnter(int param) = 0;
		virtual int Exit() = 0;
	};

	class Mutex : public Lock {
	private:
		pthread_mutex_t mutex;
	public:
		Mutex() {
			pthread_mutex_init(&this->mutex, NULL);
		}

		~Mutex() {
			pthread_mutex_destroy(&this->mutex);
		}

		virtual int Enter(int param = 0) {
			return pthread_mutex_lock(&this->mutex);
		}

		virtual int Exit() {
			return pthread_mutex_unlock(&this->mutex);
		}

		virtual int TryEnter(int param = 0) {
			return pthread_mutex_trylock(&this->mutex);
		}
	};

	class SpinLock : public Lock {
	private:
		pthread_spinlock_t spinlock;
	public:
		SpinLock() {
			pthread_spin_init(&this->spinlock, 0);
		}

		~SpinLock() {
			pthread_spin_destroy(&this->spinlock);
		}

		virtual int Enter(int param = 0) {
			return pthread_spin_lock(&this->spinlock);
		}

		virtual int Exit() {
			return pthread_spin_unlock(&this->spinlock);
		}

		virtual int TryEnter(int param = 0) {
			return pthread_spin_trylock(&this->spinlock);
		}
	};

	class RWLock : public Lock {
	private:
		pthread_rwlock_t rwlock;
	public:
		RWLock() {
			pthread_rwlock_init(&this->rwlock, NULL);
		}

		~RWLock() {
			pthread_rwlock_destroy(&this->rwlock);
		}

		/* @param: 0Îª¶ÁËø£¬1ÎªÐ´Ëø */
		virtual int Enter(int param) {
			if (param == 0)
				return pthread_rwlock_rdlock(&this->rwlock);
			else
				return pthread_rwlock_wrlock(&this->rwlock);
		}

		virtual int Exit() {
			return pthread_rwlock_unlock(&this->rwlock);
		}

		virtual int TryEnter(int param) {
			if (param == 0)
				return pthread_rwlock_tryrdlock(&this->rwlock);
			else
				return pthread_rwlock_trywrlock(&this->rwlock);
		}
	};

}

#endif