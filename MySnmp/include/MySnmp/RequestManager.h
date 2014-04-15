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
			result = new SnmpResult(type, host, requestId);

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