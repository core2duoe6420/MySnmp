#ifndef __SAFETYPE_H
#define __SAFETYPE_H

#include <MySnmp/Lock.h>
#include <queue>
#include <unordered_map>

namespace mysnmp {

	class SafeInteger {
	private:
		Lock * lock;
		int value;

		/* 禁用拷贝构造函数 */
		SafeInteger(const SafeInteger& safeint) {}

	public:
		SafeInteger(Lock * lock, int initialValue = 0) : lock(lock), value(initialValue) {}

		~SafeInteger() {
			if (this->lock)
				delete lock;
		}

		int Get() {
			int retval;
			if (this->lock)
				this->lock->Enter(0);
			retval = this->value;
			if (this->lock)
				this->lock->Exit();
			return retval;
		}

		void Set(int value) {
			if (this->lock)
				this->lock->Enter(1);
			this->value = value;
			if (this->lock)
				this->lock->Exit();
		}

		int GetAndInc() {
			int retval;
			if (this->lock)
				this->lock->Enter(0);
			retval = this->value;
			this->value++;
			if (this->lock)
				this->lock->Exit();
			return retval;
		}
	};

	template<typename T>
	class SafeQueue {
	private:
		std::queue<T> * queue;
		Lock * lock;

		/* 禁用拷贝构造函数 */
		SafeQueue(const SafeQueue<T>&) {}

	public:
		SafeQueue(Lock * lock) : lock(lock) {
			queue = new std::queue<T>();
		}

		~SafeQueue() {
			delete queue;
			if (this->lock)
				delete lock;
		}

		void Enque(const T& value) {
			if (this->lock)
				this->lock->Enter(0);

			this->queue->push(value);

			if (this->lock)
				this->lock->Exit();
		}

		T Deque() {
			T retval;

			if (this->lock)
				this->lock->Enter(0);

			retval = this->queue->front();
			this->queue->pop();

			if (this->lock)
				this->lock->Exit();

			return retval;
		}
	};

	template<typename keyType, typename valueType>
	class SafeHashMap {
		typedef std::unordered_map<keyType, valueType> HashMap;
		typedef void(*OnDeleteCallback)(const keyType&, valueType&);
	private:
		HashMap * hashmap;
		Lock * lock;
		/* 禁用拷贝构造函数 */
		SafeHashMap(const SafeHashMap<keyType, valueType>&) {}

		OnDeleteCallback onDestroyCallbackToEachItem;

	public:
		SafeHashMap(Lock * lock, OnDeleteCallback onDestroyCallbackToEachItem = NULL) :
			lock(lock), onDestroyCallbackToEachItem(onDestroyCallbackToEachItem) {
			hashmap = new HashMap();
		}

		~SafeHashMap() {
			if (this->onDestroyCallbackToEachItem)
				this->DeleteAll(this->onDestroyCallbackToEachItem);
			delete hashmap;
			if (this->lock)
				delete lock;
		}

		void SetOnDestroyCallbackToEachItem(OnDeleteCallback callback) {
			this->onDestroyCallbackToEachItem = callback;
		}

		/* @modify: 设置为true当哈希表中已有key时覆盖值
		 * @callback: 仅modify为true时有效，对被替换的项做某些操作，例如释放指针指向的内存
		 * @return: 一般情况下返回true，当且仅当哈希表中已有值并且modify为true时返回false
		 */
		bool Insert(const keyType& key, const valueType& value, bool modify = false, OnDeleteCallback callback = NULL) {
			bool retval = true;;
			if (this->lock)
				this->lock->Enter(1);

			std::pair<typename HashMap::iterator, bool> insertRet = this->hashmap->insert(typename HashMap::value_type(key, value));
			if (!insertRet.second && modify) {
				if (callback)
					callback(insertRet.first->first, insertRet.first->second);
				insertRet.first->second = value;
				retval = false;
			}

			if (this->lock)
				this->lock->Exit();
			return retval;
		}

		valueType * Find(const keyType& key) const {
			valueType * retval;
			if (this->lock)
				this->lock->Enter(0);

			typename HashMap::iterator iter = this->hashmap->find(key);
			if (iter == this->hashmap->end())
				retval = NULL;
			else
				retval = &iter->second;

			if (this->lock)
				this->lock->Exit();

			return retval;
		}

		bool Delete(const keyType& key, OnDeleteCallback onDelete = NULL) {
			bool retval;
			if (this->lock)
				this->lock->Enter(1);

			typename HashMap::iterator iter = this->hashmap->find(key);
			if (iter == this->hashmap->end()) {
				retval = false;
			} else {
				retval = true;
				if (onDelete)
					onDelete(iter->first, iter->second);
				this->hashmap->erase(iter);
			}

			if (this->lock)
				this->lock->Exit();

			return retval;
		}

		void DeleteAll(OnDeleteCallback onDelete = NULL) {
			if (this->lock)
				this->lock->Enter(1);

			if (onDelete) {
				typename HashMap::iterator iter = this->hashmap->begin();
				for (; iter != this->hashmap->end(); iter++)
					onDelete(iter->first, iter->second);
			}

			this->hashmap->clear();
			if (this->lock)
				this->lock->Exit();
		}
	};
}

#endif