#include <MySnmp/debug.h>

#include <MySnmp/RequestManager.h>
#include <MySnmp/HostManager.h>

using namespace mysnmp;

RequestManager RequestManager::singleton(Host::AddOidValueFromSnmpResult);
SafeInteger RequestManager::nextRequestId(new SpinLock(), 0);

RequestManager::RequestManager(SnmpResultHandler resultHandler = NULL) :
sem_requestWaitQueueEmpty(0), sem_resultWaitQueueEmpty(0), sem_running(10),
requestQueueHandleThread(RequestManager::requestQueueHandler),
resultQueueHandleThread(RequestManager::resultQueueHandler),
requestWaitQueue(new Mutex()),
resultWaitQueue(new Mutex()),
requestHolders(new SpinLock()),
resultHandler(resultHandler) {

	requestQueueHandleThread.Run(this);
	resultQueueHandleThread.Run(this);

}

/* 拷贝构造函数没有用，永远不要用它 */
RequestManager::RequestManager(const RequestManager& other) :
sem_requestWaitQueueEmpty(0), sem_resultWaitQueueEmpty(0), sem_running(10),
requestQueueHandleThread(RequestManager::requestQueueHandler),
resultQueueHandleThread(RequestManager::resultQueueHandler),
requestWaitQueue(new Mutex()),
resultWaitQueue(new Mutex()),
requestHolders(new SpinLock()) {}

void RequestManager::clearAllReqeustHolders() {
	requestHolders.DeleteAll([](const int& requestId, RequestHolder *& request) {
		//request->GetThread()->Cancel();
		request->GetThread()->Join(NULL);
		delete request;
	});
}

void * RequestManager::requestQueueHandler(void * data) {
	RequestManager * manager = (RequestManager *)data;
	while (1) {
		manager->sem_requestWaitQueueEmpty.Wait();
		//pthread_testcancel();
		RequestHolder * holder = manager->requestWaitQueue.Deque();
		manager->sem_running.Wait();
		holder->thread->Run(holder);
	}
	return NULL;
}

void * RequestManager::resultQueueHandler(void * data) {
	RequestManager * manager = (RequestManager *)data;
	while (1) {
		manager->sem_resultWaitQueueEmpty.Wait();
		//pthread_testcancel();
		RequestHolder * holder = manager->resultWaitQueue.Deque();

		if (manager->resultHandler)
			manager->resultHandler(holder->result);

		//请求没有完成不删除holder对象
		if (holder->IsComplete() == false)
			continue;

		manager->sem_running.Post();

		Host& host = holder->result->GetHost();
		manager->clearRequestHolder(holder->result->GetRequestId());

		/* 有两处删除Host对象的地方，这里是一处
		 * 本程序中Host的DelFlag应该只有Command类能设置
		 * 而且只有在用户发出删除命令后才能删除
		 * 因此DelFlag设置后理论上不应该再有SNMP请求
		 * 对引用计数的访问此时应该没有竞争了
		 */
		if (host.GetDelFlag() == true && host.GetReferenceCount() == 0)
			HostManager::RemoveHost(host.GetId());
	}
	return NULL;
}

void RequestManager::clearRequestHolder(int requestId) {
	this->requestHolders.Delete(requestId, [](const int& requestId, RequestHolder *& request) {
		delete request;
	});
}

void RequestManager::AddRequestToQueue(SnmpRequest * request) {
	RequestHolder ** requestHolder = NULL;
	requestHolder = requestHolders.Find(request->GetRequestId());
	if (requestHolder == NULL)
		return;
	this->requestWaitQueue.Enque(*requestHolder);
	this->sem_requestWaitQueueEmpty.Post();
}

void RequestManager::AddResultToQueue(RequestHolder * holder) {
	this->resultWaitQueue.Enque(holder);
	this->sem_resultWaitQueueEmpty.Post();
}

SnmpGetRequestBase * RequestManager::CreateSnmpGetRequest(Host& host, SnmpType type) {
	int requestId = nextRequestId.GetAndInc();
	RequestHolder * requestHolder = new RequestHolder(type, requestId, host, this);
	this->requestHolders.Insert(requestId, requestHolder);
	return dynamic_cast<SnmpGetRequestBase *>(requestHolder->request);
}

SnmpSetRequest * RequestManager::CreateSnmpSetRequest(Host& host) {
	int requestId = nextRequestId.GetAndInc();
	RequestHolder * requestHolder = new RequestHolder(SnmpType::SNMP_SET, requestId, host, this);
	this->requestHolders.Insert(requestId, requestHolder);
	return dynamic_cast<SnmpSetRequest *>(requestHolder->request);
}

////debug
//void handler(SnmpResult * result) {
//	const Snmp_pp::IpAddress& address = result->GetHost().GetAddress();
//	std::cout << "Request " << result->GetRequestId() << " : " << address.get_printable() << endl;
//	const std::vector<Snmp_pp::Vb>& vblist = result->GetVbList();
//	for (int i = 0; i < vblist.size(); i++) {
//		const Snmp_pp::Vb& vb = vblist[i];
//		std::cout << vb.get_printable_oid() << " : " << vb.get_printable_value() << endl;
//	}
//}

//#include <MySnmp/OidTree.h>
//OidTree oidtree("oid.xml");
//Host  * host;
//RequestManager manager;
//
//int snmp_start() {
//
//	//_CrtSetBreakAlloc(16878);
//
//	Snmp_pp::Snmp::socket_startup();
//	Snmp_pp::IpAddress address("192.168.157.138");
//	manager.SetResultHandler(Host::AddOidValueFromSnmpResult);
//	host = new Host(oidtree, address);
//	for (int i = 0; i < 100; i++) {
//		SnmpGetRequest * request = manager.CreateSnmpGetRequest(*host);
//		request->AddOid("1.3.6.1.2.1.1.1.0");
//		request->AddOid("1.3.6.1.2.1.1.2.0");
//		manager.AddRequestToQueue(request);
//	}
//	return 0;
//}