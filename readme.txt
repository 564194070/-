    //登录信息
    LOGIN_MSG  1,
    //登录响应消息
    LOGIN_MSG_ACK 2,
    //注册业务信息  
    REG_MSG 3,
    //注册响应消息
    REG_MSG_ACK 4,
    //聊天消息 1 - 1
    ONE_CHAT_MSG 5,


    登陆模板 {"msgid":1,"id":1,"name":"stukfov","password":"Russia"}
            {"msgid":1,"id":2,"name":"baishaolin","password":"mohican"}
    注册模板 {"msgid":3,"name":"baishaolin","password":"mohican"}
    聊天消息模板 {"msgid":5,"id":1,"from":"stukfov","toid":2,"msg":"hello"}
                {"msgid":5,"id":1,"from":"stukfov","toid":2,"msg":"666"}
                {"msgid":5,"id":1,"from":"stukfov","toid":2,"msg":"123"}
                {"msgid":5,"id":1,"from":"stukfov","toid":2,"msg":"111"}

    添加好友模板 {"msgid":6,"id":2,"friendid":1}