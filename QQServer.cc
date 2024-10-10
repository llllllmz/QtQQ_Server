#include "./QQServer.h"

#include "muduo/base/Logging.h"

#include <muduo/base/Thread.h>
#include "def_eventType.h"
#include "EventDispatcher.h"
#include "entity/User.hpp"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// using namespace muduo;
// using namespace muduo::net;
UserIdToConnMap g_userIdToConn;
std::mutex g_mutex;

QQServer::QQServer(muduo::net::EventLoop *loop,
                   const muduo::net::InetAddress &listenAddr,
                   int numEventLoops,
                   int numThreads,
                   bool nodelay)
    : server_(loop, listenAddr, "QQ_Server"),
      threadPool_(),
      numThreads_(numThreads),
      tcpNoDelay_(nodelay)
{
  LOG_INFO << "Use " << numEventLoops << " IO threads.";
  LOG_INFO << "TCP no delay " << nodelay;

  server_.setConnectionCallback(
      std::bind(&QQServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&QQServer::onMessage, this, _1, _2, _3));

  server_.setThreadNum(numEventLoops);
}

void QQServer::start()
{
  LOG_INFO << "starting " << numThreads_ << " threads.";
  threadPool_.start(numThreads_);
  server_.start();
}

void QQServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
  LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
  if (conn->connected() && tcpNoDelay_)
    conn->setTcpNoDelay(true);
    
    if(!conn->connected())
    {
      g_mutex.lock();
      User user = boost::any_cast<User>(conn->getContext());
      g_userIdToConn.erase(user.get_id());
      g_mutex.unlock();
    }
}

void QQServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                         muduo::net::Buffer *buf,
                         muduo::Timestamp time)
{
  // muduo::string msg(buf->retrieveAllAsString());
  // LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
  //          << "data received at " << time.toString()
  //          << ", threadId " << muduo::CurrentThread::tid(); // 打印线程ID
  // conn->send(msg);

  while (buf->readableBytes() >= lHeaderTotal)
  {
    const char *data = buf->peek();
    char header[sizeof(cHeaderMagic)];
    strncpy(header,data,strlen(cHeaderMagic));
    header[strlen(cHeaderMagic)]='\0';
    EventType typeNet = *(EventType *)(data + lHeaderMagic);
    int lenNet = *(int *)(data + lHeaderMagic + lHeaderEventType);
    EventType type = (EventType)muduo::net::sockets::networkToHost16(typeNet);
    int32_t len = muduo::net::sockets::networkToHost32(lenNet);

    if (len > 65535 || len < 0)
    {
      LOG_ERROR << "invalid length: "<<len
      << " header[magic: " << header << " type: " << type << " length: " << len<<"]";
      conn->shutdown();
      return;
    }
    else if (len + lHeaderTotal <= buf->readableBytes())
    {
      // 日志打印
      LOG_INFO << "success to receive a pack!"
      << " header[magic: " << header << " type: " << type << " length: " << len<<"]";
      buf->retrieve(lHeaderTotal);
      muduo::string message(buf->peek(), len);
      threadPool_.run(std::bind(&EventDispatcher::svr,EventDispatcher::getInstance(),conn,type,message));
      buf->retrieve(len);
    }
    else
    {
      break;
    }
  }
}
