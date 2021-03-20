//数据表具体的业务操作类
#ifndef USERMODEL_H_
#define USERMODEL_H_

#include"user.hpp"
// 和业务不相关 只是针对表 
// User表的数据操作类
class UserModel
{
public:
    //User表的增加方法
    bool insert(User &user);
    
    //根据用户号码查询用户信息
    User query(int id);

    //更新用户的状态信息
    bool updateState(User user);

    //重置用户的状态信息
    void resetState();

};
#endif