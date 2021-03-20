#ifndef FRIENDMODEL_H_
#define FRIENDMODEL_H_

//联合查询
#include"user.hpp"
#include<vector>
using namespace std;

//维护好友信息的操作接口方法
class FriendModel
{
public:
    //添加好友关系
    void insert(int userid,int friendid);
    
    //通过服务器返回好友列表，服务器压力过大，使用本地存储 下线的时候把好友列表同步到本地
    //返回用户好友列表
    //只返回好友id信息太少，在User表上获取friend信息传递给用户
    //通过一个用户的id找到它的用户列表
    vector<User> query(int userid);
};


#endif