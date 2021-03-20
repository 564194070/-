#ifndef PUBLIC_H_
#define PUBLIC_H_

/*
    server 和 client的公共文件
*/

enum EnMsgType
{
    //登录信息      1
    LOGIN_MSG = 1,
    //登录响应消息  2
    LOGIN_MSG_ACK,
    //注册业务信息  3
    REG_MSG,
    //注册响应消息  4
    REG_MSG_ACK,
    //聊天消息      5
    ONE_CHAT_MSG,
    //添加好友消息  6
    ADD_FRIEND_MSG,

    //创建群组      7
    CREATE_GROUP_MSG, 
    //加入群组      8
    ADD_GROUP_MSG,
    //群聊天        9
    GROUP_CHAT_MSG

};
#endif