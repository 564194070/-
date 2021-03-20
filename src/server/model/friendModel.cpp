#include"friendModel.hpp"
#include"db.h"

//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql,"insert into friend value(%d,%d)",userid,friendid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

//通过服务器返回好友列表，服务器压力过大，使用本地存储 下线的时候把好友列表同步到本地
//返回用户好友列表
//只返回好友id信息太少，在User表上获取friend信息传递给用户
//通过一个用户的id找到它的用户列表
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};
    //friend和user联合查询 userid->friend id
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid=a.id where b.userid=%d", userid);

    vector<User> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setName(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}