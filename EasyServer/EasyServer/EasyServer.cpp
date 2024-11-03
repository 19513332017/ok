#include <iostream>
using namespace std;
#include<WinSock2.h>
#include<Windows.h>
#include< Ws2tcpip.h >

enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};
struct DataHead
{
    short dataLength;
    short cmd;
};
struct Login :public DataHead
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char passWord[32];
};
struct LoginResult :public DataHead
{
    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};
struct Logout :public DataHead
{
    Logout()
    {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};
struct LogoutResult :public DataHead
{
    LogoutResult()
    {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;
    }
    int result;
};

//#pragma comment(lib,"ws2_32.lib")   附加依赖项
int main()
{
    //1.创建socket
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2),&data);   //初始化winSocket库
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        cerr << "error,sock" << endl;
    }
   
    //2.绑定端口bind

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
    {
        cerr << "error bind"   <<endl;
    }
   
    //3.监听网络端口listen

    if (listen(sock, 5) == -1)
    {
        cerr << "error,listen" << endl;
    }

    //4.等待客户端连接 accept
    sockaddr_in caddr;
    int caddrLen = sizeof(caddr);
    SOCKET clientSocket;

    clientSocket = accept(sock, reinterpret_cast<sockaddr*>(&caddr), &caddrLen);
    if (clientSocket == -1)
    {
        cerr << "接收连接失败" << endl;
    }
    else
    {
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(caddr.sin_addr), clientIP, INET_ADDRSTRLEN);   //inet_ntop是转换ip的
        cerr << "接收连接成功，客户端 IP: " << clientIP <<endl;   //打印出ip信息
    }
    
    while (true)     //一直循环接收客户端信息
    {
       //5.接受客户端数据
        DataHead header = {};
        int len=recv(clientSocket, (char*) & header, sizeof(DataHead), 0);
        if (len <= 0)
        {
            cerr << "客户端已离开，任务结束" << endl;
            break;
        }
       
        //6.处理请求
        switch (header.cmd)
        {
        case CMD_LOGIN:
        {
            Login login = {};
            recv(clientSocket, (char*)&login+sizeof(DataHead), sizeof(Login)-sizeof(DataHead), 0);    //接收客户端发的信息
            cerr << "收到命令：CMD_LOGIN  数据长度：" << login.dataLength << "用户名：" << login.userName <<"密码:"<<login.passWord<<endl;
            //忽略一步判断密码是否正确
            LoginResult ret;
            send(clientSocket, (char*)&ret, sizeof(LoginResult), 0);      //发送数据
        }
        break;
        case CMD_LOGOUT:
        {
            Logout logout = {};
            recv(clientSocket, (char*)&logout+sizeof(DataHead), sizeof(Logout)-sizeof(DataHead), 0);   //接收客户端发的信息
            cerr << "收到命令：CMD_LOGOUT  数据长度：" << logout.dataLength << "用户名：" << logout.userName << endl;
            LoginResult ret;
            send(clientSocket, (char*)&ret, sizeof(LogoutResult), 0);      //发送数据
        }
        break;
        default:
        {
            header.cmd = CMD_ERROR;
            header.dataLength = 0;
            send(clientSocket, (char*)&header, sizeof(header), 0);
        }
        break;

        }
    }
    //6.关闭套接字,清理环境
    closesocket(sock);
    WSACleanup();
    return 0;
}



