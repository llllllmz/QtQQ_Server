#pragma once
//定义事件类型
enum EventType
{
    ET_LoginRequest,    //登录请求事件
    ET_LoginResponse,   //登录响应事件
    ET_LogoutRequest,   //注销请求事件
    ET_LogoutResponse,  //注销响应事件
    ET_UserGroupRequest,//获取好友分组事件
    ET_UserGroupResponse,//获取好友分组响应事件
    ET_UserListRequest,//获取好友列表事件
    ET_UserListResponse,//获取好友列表响应事件
    ET_GroupListRequest,//获取群列表事件
    ET_GroupListResponse,//获取群列表响应事件
    ET_GroupMembersRequest,//获取群成员事件
    ET_GroupMembersResponse,//获取群成员响应事件
    //消息事件
    ET_MessageSendRequest,
    ET_MessageSendResponse,
    ET_MessageReceive,
    ET_MessageReceiveAck,
    
    ET_MessageRead,
    //拉取未读消息
    //拉取未读群消息
    ET_UnReadMessagePullRequest,
    ET_UnReadMessagePullResponse,
    ET_MessageListRequest,
    ET_MessageListResponse,

    ET_HeartBeat,       //心跳事件
    ET_Other
};