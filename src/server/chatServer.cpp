#include "chatServer.hpp"
//需要派发服务
#include "chatService.hpp"
#include"json.hpp"
#include <functional>
#include<string>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    //注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    //注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    //设置线程数量
    _server.setThreadNum(4);
}

//启动服务
void ChatServer::start()
{
    _server.start();
}

//链接相关信息回调函数
void ChatServer::onConnection(const TcpConnectionPtr & conn)
{
    if(conn->connected())
    {

    }
    else
    {
        //释放fd资源 下线
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

//读写事件相关回调函数
void ChatServer::onMessage(const TcpConnectionPtr & conn,
                           Buffer* buffer,
                           muduo::Timestamp time)
                           {
                               string buf = buffer->retrieveAllAsString();
                               //数据的反序列化 解码
                               json js = json::parse(buf);
                               //为了不强耦合，所以在这里不能使用messageID区分消息类型
                               //通过js["msg_id"] 绑定回调操作，一个ID，一个操作 获取->业务handler->conn js time
                               //完全解耦网络模块和业务模块的代码,不要调用业务模块的方法
                               //面向接口（抽象基类的编程）
                               auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
                               msgHandler(conn,js,time);
                               //网络模块中没有业务I/O的代码了，用回调
                               //回调消息绑定好的事件处理器，来执行相应的业务处理
                               //业务代码改动不需要网络代码改动
                           }