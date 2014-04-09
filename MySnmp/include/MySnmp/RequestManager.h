#ifndef __REQUEST_MANAGER_H
#define __REQUEST_MANAGER_H

#include <MySnmp/SnmpRequest.h>
#include <MySnmp/Lock.h>
#include <MySnmp/Semaphore.h>
#include <MySnmp/Thread.h>
#include <MySnmp/SnmpResult.h>
#include <MySnmp/SafeType.h>

namespace mysnmp {

	typedef void(*SnmpResultHandler)(SnmpResult * result);

	class RequestHolder {
		friend class RequestManager;
	private:
		SnmpRequest * request;
		SnmpResult * result;
		Thread * thread;

		RequestHolder(SnmpType type, int requestId, Host& host, RequestManager * manager) {
			switch (type) {
			case SnmpType::get:
				request = new SnmpGetRequest(requestId, host, manager);
				break;
			}
			result = new SnmpResult(type, host, requestId);
			thread = new Thread(SnmpGetRequest::Run);
		}

		RequestHolder(const RequestHolder& requestHolder) {
			this->request = requestHolder.request;
			this->result = requestHolder.result;
			this->thread = requestHolder.thread;
		}

	public:
		volatile bool CancelToken;

		~RequestHolder() {
			delete request;
			delete result;
			delete thread;
		}

		SnmpResult * GetSnmpResult() const { return result; }
		Thread * GetThread() const { return thread; }
		SnmpRequest * GetSnmpRequest() const { return request; }
	};

	/* 有两个工作线程，两个队列
	 * 一个队列requestWaitQueue接受request
	 * 一个线程负责从requestWaitQueue中检查request
	 * 并对每个request创建一个线程执行
	 * 每个线程执行完毕后会通过AddResultToQueue()
	 * 将SnmpResult放入resultWaitQueue
	 * 另一个线程负责从resultWaitQueue中取出result
	 * 并交给合适的函数处理result
	 */

	/* 所有的SnmpRequest SnmpResult和对应的Thread
	 * 都对应好放在一个RequestHolder里，RequestHolder由
	 * RequestManager统一管理，通过CreateSnmpGetRequest()
	 * 内部分配并注册在requestHolders中。
	 * RequestManager析构时，requestWaitQueue和resultWaitQueue
	 * 中可能会有RequestHolder的残留，但是由于是指针所以无关紧要
	 * 我们只需把注册在哈希表中的RequestHolder全部delete就能保证
	 * 不会有内存泄露，清理内存时，会对Thread调用Join，所以如果
	 * 有超时速度会变慢，目前没有别的解决方法，因为Cancel或者终结
	 * 线程都会导致内存泄露或错误内存访问。
	 */
	class RequestManager {
	private:
		static SafeInteger nextRequestId;

		void clearRequestHolder(int requestId);

		SafeHashMap<int, RequestHolder *> requestHolders;

		SafeQueue<RequestHolder *> requestWaitQueue;
		Semaphore sem_requestWaitQueueEmpty;
		SafeQueue<RequestHolder *> resultWaitQueue;
		Semaphore sem_resultWaitQueueEmpty;

		Semaphore sem_running;

		Thread requestQueueHandleThread;
		Thread resultQueueHandleThread;

		SnmpResultHandler resultHandler;

		void clearAllReqeustHolders();

		static void * requestQueueHandler(void * data);
		static void * resultQueueHandler(void * data);

	public:
		RequestManager();

		virtual ~RequestManager() {
			requestQueueHandleThread.Cancel();
			resultQueueHandleThread.Cancel();

			//if (sem_requestQueueEmpty.GetValue() == 0)
			//	sem_requestQueueEmpty.Post();
			//if (sem_resultQueueEmpty.GetValue() == 0)
			//	sem_resultQueueEmpty.Post();
			requestQueueHandleThread.Join(NULL);
			resultQueueHandleThread.Join(NULL);


			clearAllReqeustHolders();
		}

		SnmpGetRequest * CreateSnmpGetRequest(Host& host);

		void AddRequestToQueue(SnmpRequest * request);
		void AddResultToQueue(RequestHolder * holder);

		inline void SetResultHandler(SnmpResultHandler handler) {
			this->resultHandler = handler;
		}
	};
}

#endif