#include "./QQServer.h"
#include "muduo/base/FileUtil.h"
#include <functional>  // 包含 std::bind 的头文件
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"

#include <unistd.h>

using namespace muduo;
// using namespace muduo::net;
#include "def_eventHandler.h"
#include "EventDispatcher.h"
int main()
{

  Logger::setLogLevel(Logger::DEBUG);
  // 设置日志文件路径
  // muduo::Logger::setOutput(std::bind(&muduo::FileUtil::append, "./muduo.log", std::placeholders::_1, std::placeholders::_2));
    
  EventDispatcher::getInstance()->open();

  UserEventHandler *handler = new UserEventHandler();
  
  
  LOG_INFO << "pid = " << getpid();
  muduo::net::EventLoop loop;
  muduo::net::InetAddress listenAddr(2007);
  QQServer server(&loop, listenAddr,2,2,true);
  server.start();

  loop.loop();
  
  EventDispatcher::getInstance()->close();
  delete handler;
}

