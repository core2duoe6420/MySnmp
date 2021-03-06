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
		/* 有时候一个请求会需要很长时间，例如walk一个子树
		 * 此时让每抵达几个结果就放到Result队列中
		 * 由isComplete判断请求是否结束需要清理对象
		 */
		bool isComplete;
		SnmpRequest * request;
		SnmpResult * result;
		Thread * thread;

		RequestHolder(SnmpType type, int requestId, Host& host, RequestManager * manager) :
			isComplete(false) {
			switch (type) {
			case SnmpType::SNMP_GET:
				request = new SnmpGetRequest(requestId, host, manager);
				thread = new Thread(SnmpGetRequest::Run);
				break;
			case SnmpType::SNMP_GETNEXT:
				request = new SnmpGetNextRequest(requestId, host, manager);
				thread = new Thread(SnmpGetNextRequest::Run);
				break;
			case SnmpType::SNMP_GETBULK:
				request = new SnmpGetBulkRequest(requestId, host, manager);
				thread = new Thread(SnmpGetBulkRequest::Run);
				break;
			case SnmpType::SNMP_WALK:
				request = new SnmpWalkRequest(requestId, host, manager);
				thread = new Thread(SnmpWalkRequest::Run);
				break;
			case SnmpType::SNMP_SET:
				request = new SnmpSetRequest(requestId, host, manager);
				thread = new Thread(SnmpSetRequest::Run);
				break;
			}
			result = new SnmpResult(type, host, requestId, new SpinLock());

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
		bool IsComplete() { return isComplete; }
		void SetComplete() { this->isComplete = true; }
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
		static RequestManager singleton;
		static SafeInteger nextRequestId;

		RequestManager(SnmpResultHandler resultHandler);
		RequestManager(const RequestManager& other);

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

		SnmpGetRequestBase * CreateSnmpGetRequest(Host& host, SnmpType type = SnmpType::SNMP_GET);
		SnmpSetRequest * CreateSnmpSetRequest(Host& host);

		void AddRequestToQueue(SnmpRequest * request);
		void AddResultToQueue(RequestHolder * holder);

		void SetResultHandler(SnmpResultHandler handler) {
			this->resultHandler = handler;
		}

		static RequestManager& GetManager() { return singleton; }
	};
}

#endif