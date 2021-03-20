#ifndef CHATSERVICE_H_
#define CHATSERVICE_H_

#include<muduo/net/TcpConnection.h>
#include<unordered_map>

#include<functional>
#include<mutex>
#include"json.hpp"
#include"usermodel.hpp"
#include"offlineMessageModel.hpp"
#include"friendModel.hpp"
#include"groupModel.hpp"

//using namespace placeholders;
using json = nlohmann::json;

using namespace std;
using namespace muduo;
using namespace muduo::net;

//用using 给已经存在的类型定义新的名称
//处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr& conn,json& js,Timestamp)>;

//聊天服务器业务类 给msgid映射事件回调 一个消息id 映射一个事件处理
//业务层只能看见封装好的信息 不能看见字段/SQL等
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();
    //处理登陆业务
    void login(const TcpConnectionPtr& conn,json& js,Timestamp);
    //处理注册业务
    void reg(const TcpConnectionPtr& conn,json& js,Timestamp);
    //1对1 聊天业务
    void oneChat(const TcpConnectionPtr& conn,json& js,Timestamp);
    //添加好友业务
    void addFriend(const TcpConnectionPtr& conn,json& js,Timestamp);
    //创建群组业务
    void createGroup(const TcpConnectionPtr& conn,json& js,Timestamp);
    //加入群组业务
    void addGroup(const TcpConnectionPtr& conn,json& js,Timestamp);
    //群组聊天业务
    void groupChat(const TcpConnectionPtr& conn,json& js,Timestamp);


    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);
    //服务器异常业务重置方法
    void reset();
private:
    ChatService();
    //消息处理器的表 消息ID对应处理操作
    //存储消息ID和其对应的事件的处理方法
    unordered_map<int,MsgHandler> _msgHandlerMap;

    //数据操作对象
    UserModel _userModel;
    OfflineMessageModel _offlineMessageModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    //服务器要给用户主动推送其他用户发送的消息，客户不能拉消息（不知道时机），必须是常链接服务器
    //在业务层存储connection
    //存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;

    //定义互斥锁 保证_userConnMap的线程安全
    mutex _connMutex;
};
#endif