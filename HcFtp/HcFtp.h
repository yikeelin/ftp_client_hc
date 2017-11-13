#pragma once;  

//该宏完成在dll项目内部使用__declspec(dllexport)导出  
//在dll项目外部使用时，用__declspec(dllimport)导入  
//宏DLL_IMPLEMENT在SimpleDLL.cpp中定义  
#ifdef DLL_IMPLEMENT  
#define DLL_API __declspec(dllexport)  
#else  
#define DLL_API __declspec(dllimport)  
#endif  
 
#include <fstream>  
#include <WinSock2.h>  
#include <string>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include "md5.h"


#define MSGLEN 1024
#define DATABUFLEN 8192
#define PORT 21
#define	ACTIVE_MODE_DATAPORT 6088
#define MD5BUFLEN 33

typedef struct
{
	int activeMode;
	char serverIP[16];
	char userName[100];
	char passWord[100];
	char serverFilePath[512];
	char clientFilePath[512];
	char serverFileName[512];
	char clientFileName[512];	
	char msgbuf[MSGLEN];
	char listFileName[128]; //用于检测目标文件是否存在
	char dataBuf[DATABUFLEN];
	char currentDirectory[512];
}FTP_TRANSACTION;

char* list(char*, char*, char*);

extern "C" DLL_API int logon(char* serverIP, char* userName, char* passWord,
	char* lpErrorBuf, int iErrorBufLen);

/**

*@brief: 用于上传文件到ftp服务器

*@author: Ljh

*@Parameters:
	char*	serverIP: 服务器IP地址
	char*	userName: Ftp用户名
	char*	passWord: Ftp密码
	char*	serverFilePath: 服务器路径
	char*	clientFilePath: 客户端路径
	char*	fileName:		文件名
	char*	lpErrorBuf:		用于保存错误信息的字符串
	int		iErrorBufLen	长度
	char*	lpMD5Buf		用于保存返回的文件md5串
	int		iMD5BufLen		长度
	int		mode			服务器传输模式(0:被动 1:主动)
*@exception: 

*@return: 成功:0 失败:-1(错误信息保存在lpErrorBuf中) -2(离上次调用时间过近,端口未完全释放需要重试)
*@warning:

*/
extern "C" DLL_API int upload(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath, char* fileName,
	char* lpErrorBuf, int iErrorBufLen, char* lpMD5Buf, int iMD5BufLen, int mode);

/**

*@brief: 用于从FTP服务器下载文件

*@author: Ljh

*@Parameters:
char*	serverIP: 服务器IP地址
char*	userName: Ftp用户名
char*	passWord: Ftp密码
char*	serverFilePath: 服务器路径
char*	clientFilePath: 客户端路径
char*	fileName:		文件名
char*	lpErrorBuf:		用于保存错误信息的字符串
int		iErrorBufLen	长度
char*	MD5Str			要下载文件的MD5码,用于校验下载文件的完整性
int		mode			服务器传输模式(0:被动 1:主动)
*@exception:

*@return: 成功:0 失败:-1(错误信息保存在lpErrorBuf中) -2(离上次调用时间过近,端口未完全释放需要重试)
*@warning:

*/
extern "C" DLL_API int download(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath,
	char* fileName, char* lpErrorBuf, int iErrorBufLen, char* MD5Str, int mode);

//获取文件大小
extern "C" DLL_API int get_file_size(char* fileName, char* lpFileSizeBuf, int iFileSizeBufLen,
	char* lpErrorBuf, int iErrorBufLen);

//链接服务器
extern "C" int ftp_connect(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//断开服务器
extern "C" int ftp_quit(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//改变当前目录
extern "C" int ftp_cwd(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//回到上一层目录
extern "C" int ftp_cd_up(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//创建目录
extern "C" int ftp_mkd(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//列表
extern "C" char* ftp_list(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//下载文件
extern "C" long long int ftp_retrfile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//上传文件
extern "C" int ftp_storfile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//修改文件名&移动目录
extern "C" int ftp_renamefile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//删除文件
extern "C" int ftp_deletefile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//删除目录
extern "C" int ftp_deletefolder(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//改变传输模式 type a type i
extern "C" int ftp_type(char mode, char* lpErrorBuf, int iErrorBufLen);

//使服务器进入被动模式
extern "C" int ftp_entering_passive_mode(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//使服务器进入主动模式
extern "C" int ftp_entering_active_mode(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//链接数据端口
extern "C" int ftp_connet_datasocket(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//释放资源
extern "C" void ftp_free_resource();



//从返回信息“227 Entering Passive Mode (182,18,8,37,10,25).”中  
//获取数据端口  
extern "C" int get_portnum(char* msgbuf);

//通过控制socket执行FTP命令  
extern "C" int execute_ftp_cmd(SOCKET controlSocket, char* msgbuf, int len, int stateCode);

//从返回信息中获取状态码  
extern "C" int get_statecode(char* msgbuf);

//主动模式时创建datasock监听发送命令给服务器
extern "C" int create_listensock_send2svr(int ctrl_sock);

extern "C" int iselect_mode(int lsnSocket);

extern "C" int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen);

extern "C" int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen);