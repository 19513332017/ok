#define _CRT_SECURE_NO_WARNINGS

using namespace std;
#include <iostream>
#include<WinSock2.h>
#include<windows.h>
#include<Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

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
struct Login:public DataHead
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char passWord[32];
};
struct LoginResult:public DataHead
{
    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
    }
    int result;
};
struct Logout:public DataHead
{
    Logout()
    {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};
struct LogoutResult:public DataHead
{
    LogoutResult()
    {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
    }
    int result;
};

int main()
{
    //1.socket创建
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cerr << "error,sock" << endl;
    }
    //2.连接服务器
    sockaddr_in addr;
    in_addr inAddr;
    inet_pton(AF_INET, "127.0.0.1", &inAddr);  //将127.0.0.1转为ipv4的格式并且存贮在 inAddr 地址

    addr.sin_addr = inAddr; //将转换的ip地址给sin_addr
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    int res = connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)); //
    if (res == SOCKET_ERROR)
    {
        cerr << "服务器连接失败" << endl;
    }
    else
    {
        cerr << "服务器连接成功" << endl;
    }

    while (true)
    {
        //3.输入请求命令
        char cmdBuf[128] = {};
        cin.getline(cmdBuf, sizeof(cmdBuf));
        //4.处理请求命令
        if (0 == strcmp(cmdBuf, "exit"))
        {
            cerr << "收到exit命令，任务结束" << endl;
            break;
        }
        else if (0 == strcmp(cmdBuf, "login"))
        {
            Login login;
            strcpy(login.userName, "gzs");
            strcpy(login.passWord, "123456");

            //5.向服务器发送命令
            send(sock, (char*)&login, sizeof(Login), 0);

            //接受服务器返回的数据
            LoginResult loginRet;
            recv(sock, (char*)&loginRet, sizeof(LoginResult),0);
            cerr << "LoginResult: " << loginRet.result << endl;
        }
        else if(0==strcmp(cmdBuf,"logout"))
        {
            
            Logout logout;
            strcpy(logout.userName, "gzs");
            //5.向服务器发送命令
           
            send(sock, (char*)&logout, sizeof(Logout), 0);
            //接受服务器返回的数据
            
            LoginResult logoutRet;
            recv(sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
            cerr << "LogoutResult: " << logoutRet.result << endl;
        }
        else
        {
            cerr << "收到不支持命令，任务结束" << endl;
        }

    }
    cin.get();  //防止闪屏

    //7.关闭套接字
    closesocket(sock);
    WSACleanup();
    return 0;
}
