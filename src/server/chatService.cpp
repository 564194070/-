//日志打印
#include <muduo/base/Logging.h>
#include <string>
#include <vector>
#include <map>
#include "usermodel.hpp"
#include "friendModel.hpp"
#include "chatService.hpp"
#include "public.hpp"

using namespace muduo;
using namespace std;
//注册消息以及对应的回调函数Handler
ChatService::ChatService()
{
    //业务模块和网络模块解耦的核心
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
}

ChatService *ChatService::instance()
{
    //静态懒汉式线程安全单例模式对象
    static ChatService service;
    return &service;
}

//服务器异常 业务重置方法
void ChatService::reset()
{
    //把online状态的用户设置为offline
    _userModel.resetState();
}

//发生之后做什么，但是不知道什么时候发生，需要网络模块辅助
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //将数据层和业务层代码模块分离 ORM object relation map 对象关系映射 业务层操作的都是对象 DAO层才会有数据库的操作
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);

    if (user.getPwd() == pwd && user.getId() != -1)
    {
        //已经登陆不允许重复登陆
        if (user.getState() == "online")
        {
            //用户存在不允许重复登陆
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 1;
            response["id"] = user.getId();
            response["name"] = user.getName();
            response["errmsg"] = "该账号已经登陆";
            conn->send(response.dump());
        }
        else
        {
            {
                //记录成功链接信息
                //会随着用户上下线改变，注意线程安全
                //大括号代表作用域
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }
            //登陆成功

            //更新用户状态信息 state offl;ine = online
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            //查询用户是否有离线消息
            vector<string> vec = _offlineMessageModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                //读取该用户的离线消息后，把该用户的所有离线消息删除掉
                _offlineMessageModel.remove(id);
            }

            //查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                vector<string> userVec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    userVec2.push_back(js.dump());
                }
                response["friends"] = userVec2;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        //登陆失败 用户不存在
        //用户存在，密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["error"] = 2;
        response["id"] = user.getId();
        response["name"] = user.getName();
        response["errmsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }
}
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        //成功插入 注册成功 参数id改变了 不是-1
        json respose;
        respose["msgid"] = REG_MSG_ACK;
        respose["error"] = 0;
        respose["id"] = user.getId();
        conn->send(respose.dump());
    }
    else
    {
        //插入失败 注册失败
        json respose;
        respose["msgid"] = REG_MSG_ACK;
        respose["error"] = 3;
        respose["errmsg"] = "注册失败";
        conn->send(respose.dump());
    }
}

//获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    //用中括号查询会导致副作用添加一对
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        //返回一个默认的处理器 空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msgid:" << msgid << "can not find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                //从map表删除用户链接信息
                _userConnMap.erase(it);
                break;
            }
        }
    }

    //更新用户状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            //toid 在线 转发消息
            //服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    //不在线存储离线消息表
    _offlineMessageModel.insert(toid, js.dump());
}

//添加好友业务 msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    //存储好友信息
    _friendModel.insert(userid, friendid);
}

//创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];    

    //储存新创建的群组信息
    Group group(-1,name,desc);
    if(_groupModel.createGroup(group))
    {
        //储存群组创建人信息
        _groupModel.addGroup(userid,group.getId(),"creator");
    }
}
//加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid,groupid,"normal");
}


//群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    vector<int> useridVec =_groupModel.queryGroupsUsers(userid,groupid);

    //要不发消息之前走，要不收完消息之后走
    lock_guard<mutex> lock(_connMutex);
    for(int id:useridVec)
    {
        
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end())
        {
            //转发群消息
            it->second->send(js.dump());
        }
        else
        {
            //储存离线消息
            _offlineMessageModel.insert(id,js.dump());
        }
    }
}