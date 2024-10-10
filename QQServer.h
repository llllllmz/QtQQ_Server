#ifndef MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include "muduo/net/TcpServer.h"
#include "muduo/base/Thread.h"
#include "muduo/base/ThreadPool.h"
#include <cstring>
  
#define   cHeaderMagic       "QTQQ"
#define   lHeaderMagic       strlen(cHeaderMagic)
#define   lHeaderEventType   sizeof(int16_t)
#define   lHeaderLen         sizeof(int32_t)
#define   lHeaderTotal       (lHeaderMagic + lHeaderEventType + lHeaderLen)

using namespace muduo;
using namespace muduo::net;

#include <unordered_map>
#include <mutex>

typedef std::unordered_map<int, muduo::net::TcpConnectionPtr> UserIdToConnMap;
extern UserIdToConnMap g_userIdToConn;
extern std::mutex g_mutex;

// RFC 862
class QQServer
{
 public:
  QQServer(muduo::net::EventLoop* loop,
             const muduo::net::InetAddress& listenAddr,
             int numEventLoops,
             int numThreads, 
             bool nodelay);

  void start();  // calls server_.start();
  static std::string getCurrentTime()
  {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
    return std::string(buffer);
  }

 private:
  void onConnection(const muduo::net::TcpConnectionPtr& conn);

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp time);

  muduo::net::TcpServer server_;
  int numThreads_;
  ThreadPool threadPool_;
  const bool tcpNoDelay_;
  
};

#endif  // MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
