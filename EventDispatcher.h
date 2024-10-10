#pragma once
#include <muduo/net/TcpConnection.h>
#include "def_eventType.h"
#include "iEventHandler.h"
#include "iEvent.h"
#include <map>
#include <vector>
using namespace muduo;

struct header
{
	char magic[4];
	short type;
	int len;
};


class EventDispatcher
{
	// Singleton
private:
	EventDispatcher(){};
	
public:
	static EventDispatcher* getInstance();
	bool open();
    void close();
	//订阅
	void subscribe(EventType et, iEventHandler* handler);
    void unsubscribe(EventType et, iEventHandler* handler);

	iEvent* parseEvent(const muduo::net::TcpConnectionPtr& conn,EventType eventTpye, const muduo::string& message);
	void svr(const muduo::net::TcpConnectionPtr& conn,EventType eventType, const muduo::string message);
	iEvent* dispatch(const iEvent* ev);
	void responseEvent(const muduo::net::TcpConnectionPtr& conn,const iEvent* ev);


private:
	//响应客户端


private:
    typedef std::vector<iEventHandler*> T_EventHandlers;
    typedef std::map<EventType, T_EventHandlers > T_EventHandlersMap;
	//一个事件类型可以有多个处理者
	T_EventHandlersMap m_handlers;
	bool m_svr_exit;
public:
	static EventDispatcher* instance;	
};


