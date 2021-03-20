//使用ORM框架的第一步，需要定义相关的一些类，
//和数据库中的表一一对应，可以将数据库中读出的字段，合称为一个对象提交给业务方使用
//属于映射类

#ifndef USER_H_
#define USER_H_

#include <string>
using namespace std;

class User
{
public:
    User(int id = 1, string name = "", string pwd = "", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }
    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPwd(string pwd) { this->password = pwd; }
    void setState(string state) { this->state = state; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getPwd() { return this->password; }
    string getState() { return this->state; }

private:
    int id;
    string name;
    string password;
    string state;
};

#endif
