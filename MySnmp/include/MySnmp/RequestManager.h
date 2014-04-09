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

	/* �����������̣߳���������
	 * һ������requestWaitQueue����request
	 * һ���̸߳����requestWaitQueue�м��request
	 * ����ÿ��request����һ���߳�ִ��
	 * ÿ���߳�ִ����Ϻ��ͨ��AddResultToQueue()
	 * ��SnmpResult����resultWaitQueue
	 * ��һ���̸߳����resultWaitQueue��ȡ��result
	 * ���������ʵĺ�������result
	 */

	/* ���е�SnmpRequest SnmpResult�Ͷ�Ӧ��Thread
	 * ����Ӧ�÷���һ��RequestHolder�RequestHolder��
	 * RequestManagerͳһ����ͨ��CreateSnmpGetRequest()
	 * �ڲ����䲢ע����requestHolders�С�
	 * RequestManager����ʱ��requestWaitQueue��resultWaitQueue
	 * �п��ܻ���RequestHolder�Ĳ���������������ָ�������޹ؽ�Ҫ
	 * ����ֻ���ע���ڹ�ϣ���е�RequestHolderȫ��delete���ܱ�֤
	 * �������ڴ�й¶�������ڴ�ʱ�����Thread����Join���������
	 * �г�ʱ�ٶȻ������Ŀǰû�б�Ľ����������ΪCancel�����ս�
	 * �̶߳��ᵼ���ڴ�й¶������ڴ���ʡ�
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