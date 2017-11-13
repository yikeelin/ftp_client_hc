#define DLL_IMPLEMENT 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "HcFtp.h"
#pragma comment(lib,"WS2_32.lib")  
using namespace std;


/* 返回码
00: 成功
01: Init failed
02: Creating Control Socket failed
03: Control Socket connecting failed
04: Control Socket connecting StateCode
05: Send USERNAME failed
06: Verify UserName Or PassWord failed
07: entering passive mode failed
10: Data Socket connecting failed
11: No such file or directory(改变目录失败)
12: Permission denied. 权限不足
13: The file pointer is NULL(文件不存在)
14: RECV From Server failed, Connection Closed
15: RETR File not found
16: Get File Size failed
17: Type A I failed
18: %s", "Error: LIST failed
20: check MD5 failed, plese retry
*/

FTP_TRANSACTION pstTransaction;
SOCKET controlSocket, dataSocket, listenSocket;
SOCKADDR_IN serverAddr;
int dataPort, ret, stateCode;
long long int llMissionFileSize;
long long int llCompleteFileSize;


//char * list(char* serverIP, char* userName, char* passWord)
//{
//	char *resultMsg;
//	resultMsg = (char*)malloc(1024);
//	memset(resultMsg, 0, 1024);
//	memset(&pstTransaction, 0, sizeof(FTP_TRANSACTION));
//	pstTransaction.activeMode = 0;
//	memcpy(pstTransaction.serverIP, serverIP, strlen(serverIP));
//	memcpy(pstTransaction.userName, userName, strlen(userName));
//	memcpy(pstTransaction.passWord, passWord, strlen(passWord));
//	//链接服务器并登录
//	if (ftp_connect(&pstTransaction, resultMsg, 1024))
//	{
//		return resultMsg;
//	}
//	memset(resultMsg, 0, 1024);
//	memcpy(pstTransaction.listFileName, "", 1);
//
//	return ftp_list(&pstTransaction, resultMsg, 1024);
//	//释放资源
//	ftp_free_resource();
//}

extern "C" DLL_API int getMissionPercent(int iNewMissionFlag)
{
	if (iNewMissionFlag)
	{
		llCompleteFileSize = 0;
		llMissionFileSize = 0;
	}
	if (llMissionFileSize != 0)
	{
		int iResult = llCompleteFileSize / (llMissionFileSize / 100);
		if (iResult >= 100)
		{
			iResult = 100;
		}

		return iResult;
	}
	else
	{
		return 0;
	}

}

extern "C" DLL_API int logon(char* serverIP, char* userName, char* passWord,
	char* lpErrorBuf, int iErrorBufLen)
{
	memset(&pstTransaction, 0, sizeof(FTP_TRANSACTION));
	pstTransaction.activeMode = 0;
	memcpy(pstTransaction.serverIP, serverIP, strlen(serverIP));
	memcpy(pstTransaction.userName, userName, strlen(userName));
	memcpy(pstTransaction.passWord, passWord, strlen(passWord));
	//链接服务器并登录
	if (ftp_connect(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		return -1;
	}
	//释放资源
	ftp_free_resource();
	return 0;
}

extern "C" DLL_API int upload(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath, char* fileName,
	char* lpErrorBuf, int iErrorBufLen, char* lpMD5Buf, int iMD5BufLen, int mode)
{
	memset(lpErrorBuf, 0, iErrorBufLen);
	if (iMD5BufLen < MD5BUFLEN)
	{
		memcpy(lpErrorBuf, "Md5Buf is to short.", iErrorBufLen);
		return -1;
	}

	memset(&pstTransaction, 0, sizeof(FTP_TRANSACTION));
	pstTransaction.activeMode = mode;
	memcpy(pstTransaction.serverIP, serverIP, strlen(serverIP));
	memcpy(pstTransaction.userName, userName, strlen(userName));
	memcpy(pstTransaction.passWord, passWord, strlen(passWord));
	memcpy(pstTransaction.serverFilePath, serverFilePath, strlen(serverFilePath));
	memcpy(pstTransaction.clientFilePath, clientFilePath, strlen(clientFilePath));
	memcpy(pstTransaction.serverFileName, fileName, strlen(fileName));
	memcpy(pstTransaction.clientFileName, fileName, strlen(fileName));
	//链接服务器并登录
	if (ftp_connect(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return -1;
	}

	ftp_type('A', lpErrorBuf, iErrorBufLen);

	if (mode){
		//使服务器进入主动模式
		if (ftp_entering_active_mode(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}
	}
	else {

		//使服务器进入被动模式
		if (ftp_entering_passive_mode(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}

		//链接数据端口
		if (ftp_connet_datasocket(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}
	}

	//改变服务器当前路径
	if (ftp_cwd(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return -1;
	}

	ftp_type('I', lpErrorBuf, iErrorBufLen);
	//上传文件
	int iret;
	if (iret = ftp_storfile(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return iret;
	}




	//释放资源
	ftp_free_resource();

	//返回MD5
	memset(lpMD5Buf, 0, iMD5BufLen);
	char filePath[512];
	memset(filePath, 0, 512);
	strcpy(filePath, pstTransaction.clientFilePath);
	int index = strlen(filePath) - 1;
	if (filePath[index] != '/' && filePath[index] != '\\')
	{
		strcat(filePath, "/");
	}
	strcat(filePath, pstTransaction.clientFileName);

	if (getFileMD5(filePath, lpMD5Buf))
	{
		memcpy(lpErrorBuf, "getMd5 failed. plese retry.", iErrorBufLen);
		return -1;
	}

	return 0;
}

extern "C" DLL_API int download(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath,
	char* fileName, char* lpErrorBuf, int iErrorBufLen, char* MD5Str, int mode)
{
	memset(&pstTransaction, 0, sizeof(FTP_TRANSACTION));
	pstTransaction.activeMode = mode;
	memcpy(pstTransaction.serverIP, serverIP, strlen(serverIP));
	memcpy(pstTransaction.userName, userName, strlen(userName));
	memcpy(pstTransaction.passWord, passWord, strlen(passWord));
	memcpy(pstTransaction.serverFilePath, serverFilePath, strlen(serverFilePath));
	memcpy(pstTransaction.clientFilePath, clientFilePath, strlen(clientFilePath));
	memcpy(pstTransaction.serverFileName, fileName, strlen(fileName));
	memcpy(pstTransaction.clientFileName, fileName, strlen(fileName));
	//链接服务器并登录
	if (ftp_connect(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return -1;
	}

	ftp_type('A', lpErrorBuf, iErrorBufLen);

	if (mode){
		//使服务器进入主动模式
		if (ftp_entering_active_mode(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}
	}
	else {
		//使服务器进入被动模式
		if (ftp_entering_passive_mode(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}

		//链接数据端口
		if (ftp_connet_datasocket(&pstTransaction, lpErrorBuf, iErrorBufLen))
		{
			ftp_free_resource();
			return -1;
		}
	}

	//改变服务器当前路径
	if (ftp_cwd(&pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return -1;
	}

	//获取文件大小
	char sizeBuf[16];
	char filePath[512];
	memset(filePath, 0, 512);
	strcpy(filePath, pstTransaction.serverFilePath);
	int index = strlen(filePath) - 1;
	if (filePath[index] != '/' && filePath[index] != '\\')
	{
		strcat(filePath, "/");
	}
	strcat(filePath, pstTransaction.serverFileName);
	if (get_file_size(filePath, sizeBuf, 16, lpErrorBuf, iErrorBufLen))
	{
		ftp_free_resource();
		return -1;
	}

	long long int fsizeSource = atoll(sizeBuf);
	llMissionFileSize = fsizeSource;

	ftp_type('I', lpErrorBuf, iErrorBufLen);
	//下载文件
	long long int fsizeDest = ftp_retrfile(&pstTransaction, lpErrorBuf, iErrorBufLen);

	if (fsizeDest < 0){
		int iret = fsizeDest;  //-1:出错, -2:端口处于TIME_WAIT状态,重试.
		ftp_free_resource();
		return iret;
	}

	if (fsizeDest < fsizeSource){
		memset(lpErrorBuf, 0, iErrorBufLen);
		memcpy(lpErrorBuf, "download error, plese try again.", iErrorBufLen);
		ftp_free_resource();
		return -1;
	}

	//释放资源
	ftp_free_resource();

	//校验MD5
	char lpMD5Buf[MD5BUFLEN];
	memset(lpMD5Buf, 0, MD5BUFLEN);
	//char filePath[512];  上面声明过了
	memset(filePath, 0, 512);
	strcpy(filePath, pstTransaction.clientFilePath);
	index = strlen(filePath) - 1;
	if (filePath[index] != '/' && filePath[index] != '\\'){
		strcat(filePath, "/");
	}
	strcat(filePath, pstTransaction.clientFileName);

	if (MD5Str)				// && MD5Str[0] != '\0')
	{
		if (getFileMD5(filePath, lpMD5Buf))
		{
			memcpy(lpErrorBuf, "getMd5 failed. plese retry.", iErrorBufLen);
			return -1;
		}
		if (strcmp(lpMD5Buf, MD5Str))
		{
			memcpy(lpErrorBuf, "verify Md5 failed. plese retry.", iErrorBufLen);

			if (remove(filePath))
			{
				strcat(lpErrorBuf, "  and remove file failed");
			}
			return -1;
		}
	}

	return 0;
}

//列表
extern "C" char* ftp_list(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	if (ftp_type('a', lpErrorBuf, iErrorBufLen))
	{
		return 0;
	}

	sprintf(pstTransaction->msgbuf, "PASV\r\n");
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 227))
	{
		return 0;
	}

	//链接数据端口
	if (ftp_connet_datasocket(pstTransaction, lpErrorBuf, iErrorBufLen))
	{
		return 0;
	}
	if (pstTransaction->listFileName)
	{
		sprintf(pstTransaction->msgbuf, "LIST %s\r\n", pstTransaction->listFileName);
		memset(pstTransaction->listFileName, 0, sizeof(pstTransaction->listFileName));
	}
	else
	{
		sprintf(pstTransaction->msgbuf, "LIST\r\n");
	}

	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 150))               //230
	{
		memset(lpErrorBuf, 0, iErrorBufLen);
		memcpy(lpErrorBuf, "Error: LIST failed", iErrorBufLen);
		return 0;
	}

	char strList[DATABUFLEN * 10];
	memset(strList, 0, DATABUFLEN * 10);
	int index = 0;
	int iResult;
	do {
		//memset(pstTransaction->dataBuf, 0, sizeof(pstTransaction->dataBuf));
		iResult = recv(dataSocket, pstTransaction->dataBuf, sizeof(pstTransaction->dataBuf), 0);
		if (iResult<0)
		{
			//sprintf(resultMsg, "14| %s", "RECV From Server failed, Connection Closed ??");
			memset(lpErrorBuf, 0, iErrorBufLen);
			memcpy(lpErrorBuf, "RECV From Server failed, Connection Closed ??", iErrorBufLen);
			return 0;
		}

		memcpy(strList + index, pstTransaction->dataBuf, iResult);
		index += iResult;
	} while (iResult > 0);

	//int nRetLen;
	//nRetLen = UTF8ToGBK((unsigned char *)strList, NULL, NULL); 
	//char *lpGBKStr;
	//lpGBKStr = new char[nRetLen + 1];
	//nRetLen = UTF8ToGBK((unsigned char *)strList, (unsigned char *)lpGBKStr, nRetLen);
	//return lpGBKStr;

	return strList;
}

extern "C" DLL_API int get_file_size(char* filePath, char* lpFileSizeBuf, int iFileSizeBufLen,
	char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);

	char msgbuf[MSGLEN];
	memset(msgbuf, 0, MSGLEN);
	sprintf(msgbuf, "SIZE %s\r\n", filePath);
	if (execute_ftp_cmd(controlSocket, msgbuf, MSGLEN, 213))                //213
	{
		//sprintf(resultMsg, "16| %s", "Error: Get File Size failed");
		memcpy(lpErrorBuf, "Error: Get File Size failed, File not exist?", iErrorBufLen);
		return -1;
	}

	/*int index = 0;
	for (int i = 0; i < strlen(msgbuf); i++)
	{
	if (msgbuf[i] == ' '&& i != 0)
	{
	index = i;
	break;
	}
	}*/
	if (iFileSizeBufLen < strlen(msgbuf))
	{
		//memset(lpErrorBuf, 0, iErrorBufLen);
		memcpy(lpErrorBuf, "lpFileSizeBuf to short", iErrorBufLen);
		return -1;
	}
	memset(lpFileSizeBuf, 0, iFileSizeBufLen);
	strcpy(lpFileSizeBuf, msgbuf + 4);
	return 0;
}

extern "C" int ftp_connect(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);
	WSADATA dat;
	//初始化，很重要  
	if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)  //Windows Sockets Asynchronous启动  
	{
		memcpy(lpErrorBuf, " Init failed ", iErrorBufLen);
		return -1;
	}
	//创建Socket   
	controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (controlSocket == INVALID_SOCKET)
	{
		memcpy(lpErrorBuf, "Creating Control Socket failed", iErrorBufLen);
		return -1;
	}

	//构建服务器访问参数结构体  
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(pstTransaction->serverIP); //地址  
	serverAddr.sin_port = htons(PORT);            //端口  
	memset(serverAddr.sin_zero, 0, sizeof(serverAddr.sin_zero));
	//连接  
	ret = connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		memcpy(lpErrorBuf, "Control Socket connecting failed", iErrorBufLen);
		return -1;
	}
	//接收返回状态信息  
	recv(controlSocket, pstTransaction->msgbuf, MSGLEN, 0);
	//cout << pstTransaction->msgbuf;                                                    //220  
	//根据返回信息提取状态码，进行判断  
	if (get_statecode(pstTransaction->msgbuf) != 220)
	{
		memcpy(lpErrorBuf, "Control Socket Connecting failed", iErrorBufLen);
		return -1;
	}
	// Connect succeed;


	//用户名  
	memset(pstTransaction->msgbuf, 0, MSGLEN);
	sprintf(pstTransaction->msgbuf, "USER %s\r\n", pstTransaction->userName);
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 331))               //331  
	{
		memcpy(lpErrorBuf, "Error: Send USERNAME failed", iErrorBufLen);
		return -1;
	}

	//密码  
	memset(pstTransaction->msgbuf, 0, MSGLEN);
	sprintf(pstTransaction->msgbuf, "PASS %s\r\n", pstTransaction->passWord);
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 230))               //230
	{
		memcpy(lpErrorBuf, "Verify UserName Or PassWord failed", iErrorBufLen);
		return -1;
	}

	return 0;
}

//改变传输模式 type a type i
extern "C" int ftp_type(char mode, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);
	char msgbuf[128];
	memset(msgbuf, 0, 128);
	sprintf(msgbuf, "TYPE %c\r\n", mode);
	if (execute_ftp_cmd(controlSocket, msgbuf, 128, 200))                //200
	{
		memcpy(lpErrorBuf, "Type A I failed !200", iErrorBufLen);
		return -1;
	}
	//cout << pstTransaction->msgbuf << __LINE__ << endl;
	return 0;
}

//使服务器进入被动模式
extern "C" int ftp_entering_passive_mode(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);

	//切换到被动模式  
	memset(pstTransaction->msgbuf, 0, MSGLEN);
	sprintf(pstTransaction->msgbuf, "PASV\r\n");
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 227))               //227
	{
		memcpy(lpErrorBuf, "Error: entering passive mode failed !227", iErrorBufLen);
		return -1;
	}
	//cout << pstTransaction->msgbuf << __LINE__ << endl;
	return 0;

}

//使服务器进入主动模式
extern "C" int ftp_entering_active_mode(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);
	//切换到主动模式  
	listenSocket = create_listensock_send2svr(controlSocket);

	if (listenSocket < 0)
		return -1;
	//cout << pstTransaction->msgbuf << __LINE__ << endl;
	return 0;
}

//链接数据端口
extern "C" int ftp_connet_datasocket(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);
	//返回的信息格式为---h1,h2,h3,h4,p1,p2  
	//其中h1,h2,h3,h4为服务器的地址，p1*256+p2为数据端口  
	//cout << pstTransaction->msgbuf << __LINE__ << endl;
	dataPort = get_portnum(pstTransaction->msgbuf);
	//客户端数据传输socket  
	dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverAddr.sin_port = htons(dataPort);    //更改连接参数中的port值  
	ret = connect(dataSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		memcpy(lpErrorBuf, "Data socket connecting failed", iErrorBufLen);
		return -1;
	}
	//cout << "Data Socket connecting is success." << endl;
	return 0;
}

//改变当前目录
extern "C" int ftp_cwd(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);

	memset(pstTransaction->msgbuf, 0, MSGLEN);
	sprintf(pstTransaction->msgbuf, "CWD %s\r\n", pstTransaction->serverFilePath);   //250  
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 250))                //250
	{
		memcpy(lpErrorBuf, "Error: No such file or directory!550", iErrorBufLen);
		return -1;
	}
	return 0;
}




/*
	*下载文件
	*return : 下载的文件大小
	*/
extern "C" long long int ftp_retrfile(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	long long int result = -1;
	memset(lpErrorBuf, 0, iErrorBufLen);

	char cfile[1024];
	memset(cfile, 0, sizeof(cfile));
	int iLastIndex;
	iLastIndex = strlen(pstTransaction->clientFilePath) - 1;
	if (pstTransaction->clientFilePath[iLastIndex] == '/')
	{
		sprintf(cfile, "%s%s", pstTransaction->clientFilePath, pstTransaction->clientFileName);
	}
	else
	{
		sprintf(cfile, "%s/%s", pstTransaction->clientFilePath, pstTransaction->clientFileName);
	}


	llCompleteFileSize = 0;  //用于记录传输完成的数据总数
	//检测是否存在未传输完的文件
	int iFileExist = _access(cfile, 0);
	FILE* frb;
	if (!iFileExist)
		frb = fopen(cfile, "rb+"); //续传
	else
		frb = fopen(cfile, "wb");	//新建
	if (frb)
	{
		if (!iFileExist)  //续传
		{
			_fseeki64(frb, 0, SEEK_END);
			long long int ioffset = _ftelli64(frb);
			llCompleteFileSize = ioffset;
			//断点续传
			memset(pstTransaction->msgbuf, 0, MSGLEN);
			sprintf(pstTransaction->msgbuf, "REST %lld\r\n", ioffset);
			if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 350))                //250
			{
				memcpy(lpErrorBuf, "restarting set offset failed", iErrorBufLen);
				return -1;
			}
		}
		int ret;



		//下载文件 
		memset(pstTransaction->msgbuf, 0, MSGLEN);
		sprintf(pstTransaction->msgbuf, "RETR %s\r\n", pstTransaction->serverFileName);
		if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 150))                //250
		{
			memcpy(lpErrorBuf, "File not found", iErrorBufLen);
			//550 Permission denied. 权限不足
			return -1;
		}

		//主动模式accept 监听中的datasocket
		if (pstTransaction->activeMode)	{
			sockaddr_in sin;
			int len = sizeof(sin);

			//dataSocket = accept(listenSocket, (sockaddr *)&sin, &len);
			dataSocket = iselect_mode(listenSocket);
			if (dataSocket == INVALID_SOCKET){
				memcpy(lpErrorBuf, "accept failed", iErrorBufLen);
				return -1;
			}
			if (dataSocket == 0){	//端口释放未完成 TIME_WAIT状态
				memcpy(lpErrorBuf, "dataport may be in TIME_WAIT, Try again.", iErrorBufLen);
				return -2;
			}
		}

		do {
			//memset(pstTransaction->dataBuf, 0, sizeof(pstTransaction->dataBuf));
			ret = recv(dataSocket, pstTransaction->dataBuf, sizeof(pstTransaction->dataBuf), 0);
			if (ret<0)
			{
				memcpy(lpErrorBuf, "RECV From server failed, Connection closed ??", iErrorBufLen);
				return -1;
			}
			fwrite(pstTransaction->dataBuf, 1, ret, frb);
			llCompleteFileSize += ret;
		} while (ret > 0);
		result = _ftelli64(frb);
		fclose(frb);
	}
	else{
		memcpy(lpErrorBuf, "The file pointer is NULL", iErrorBufLen);
		return -1;
	}
	return result;
}

//上传文件
extern "C" int  ftp_storfile(FTP_TRANSACTION *pstTransaction, char* lpErrorBuf, int iErrorBufLen)
{
	memset(lpErrorBuf, 0, iErrorBufLen);

	//读取本地文件
	int iLastIndex;

	char cfile[1024];
	memset(cfile, 0, sizeof(cfile));

	iLastIndex = strlen(pstTransaction->clientFilePath) - 1;
	if (pstTransaction->clientFilePath[iLastIndex] == '/' || pstTransaction->clientFilePath[iLastIndex] == '\\')
	{
		sprintf(cfile, "%s%s", pstTransaction->clientFilePath, pstTransaction->clientFileName);
	}
	else
	{
		sprintf(cfile, "%s/%s", pstTransaction->clientFilePath, pstTransaction->clientFileName);
	}
	//cout << cfile << endl;
	FILE* f = fopen(cfile, "rb");
	if (f == NULL)
	{
		memcpy(lpErrorBuf, "The file pointer is NULL! file not exist", iErrorBufLen);
		return -1;
	}



	//发送上传命令
	memset(pstTransaction->msgbuf, 0, MSGLEN);
	sprintf(pstTransaction->msgbuf, "STOR %s\r\n", pstTransaction->serverFileName);
	if (execute_ftp_cmd(controlSocket, pstTransaction->msgbuf, MSGLEN, 150))                //150
	{
		memcpy(lpErrorBuf, "550 Permission denied, 'stor' failed", iErrorBufLen);
		//550 Permission denied. 权限不足
		return -1;
	}

	//主动模式accept 监听中的datasocket
	if (pstTransaction->activeMode)	{
		sockaddr_in sin;
		int len = sizeof(sin);

		//dataSocket = accept(listenSocket, (sockaddr *)&sin, &len);
		dataSocket = iselect_mode(listenSocket);
		if (dataSocket == INVALID_SOCKET){
			memcpy(lpErrorBuf, "accept failed", iErrorBufLen);
			return -1;
		}
		if (dataSocket == 0){	//端口释放未完成 TIME_WAIT状态
			memcpy(lpErrorBuf, "dataport may be in TIME_WAIT, Try again.", iErrorBufLen);
			return -2;
		}
	}

	int iResult = 0;
	while (!feof(f))
	{
		//int optval;
		//int optlen = sizeof(int);
		//int r;
		//memset(pstTransaction->dataBuf, 0, sizeof(pstTransaction->dataBuf));
		//r = getsockopt(dataSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
		//cout << r << endl << optval << endl ;
		iResult = fread(pstTransaction->dataBuf, 1, sizeof(pstTransaction->dataBuf), f);
		send(dataSocket, pstTransaction->dataBuf, iResult, 0);
	}
	fclose(f);
	return 0;
}

extern "C" void ftp_free_resource()
{
	Sleep(1000);
	//释放资源  
	if (listenSocket)
	{
		//cout << "close listenSocket:" << listenSocket << endl;
		closesocket(listenSocket);
	}
	if (dataSocket)
	{
		//cout << "close dataSocket:" << dataSocket << endl;
		closesocket(dataSocket);
	}
	if (controlSocket)
	{
		//cout << "close controlSocket:" << controlSocket << endl;
		closesocket(controlSocket);
	}
	WSACleanup();
}



//从返回信息“227 Entering Passive Mode (182,18,8,37,10,25).”中  
//获取数据端口  
extern "C" int get_portnum(char* msgbuf)
{
	int num1 = 0, num2 = 0;

	char* p = msgbuf;
	int cnt = 0;
	while (1)
	{
		if (cnt == 4 && (*p) != ',')
		{
			num1 = 10 * num1 + (*p) - '0';
		}
		if (cnt == 5)
		{
			num2 = 10 * num2 + (*p) - '0';
		}
		if ((*p) == ',')
		{
			cnt++;
		}
		p++;
		if ((*p) == ')')
		{
			break;
		}
	}
	return num1 * 256 + num2;
}
//通过控制socket执行FTP命令  
extern "C" int execute_ftp_cmd(SOCKET controlSocket, char* msgbuf, int len, int stateCode)
{
	send(controlSocket, msgbuf, strlen(msgbuf), 0);
	memset(msgbuf, 0, len);
	Sleep(100);
	recv(controlSocket, msgbuf, len > 512 ? 512 : len, 0);
	//cout << msgbuf;
	if (get_statecode(msgbuf) == stateCode)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
//从返回信息中获取状态码  
extern "C" int get_statecode(char* msgbuf)
{
	int num = 0;
	char* p = msgbuf;
	while (p != NULL)
	{
		num = 10 * num + (*p) - '0';
		p++;
		if (*p > '9' || *p < '0')
		{
			break;
		}
	}
	//cout << num << endl;
	return num;
}

extern "C" int create_listensock_send2svr(int ctrl_sock)
{
	int     lsn_sock;
	int     port;
	int     len;
	struct sockaddr_in sin;
	char    cmd[1024];
	int		iSockReuse;



	lsn_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//设置端口复用
	iSockReuse = 1;
	setsockopt(lsn_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&iSockReuse, sizeof(iSockReuse));

	if (lsn_sock == -1)
		return -1;
	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	port = ACTIVE_MODE_DATAPORT;
	sin.sin_port = htons(port);
	if (bind(lsn_sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		closesocket(lsn_sock);
		return -1;
	}

	if (listen(lsn_sock, 2) == -1) {
		closesocket(lsn_sock);
		return -1;
	}



	len = sizeof(struct sockaddr);
	if (getsockname(lsn_sock, (struct sockaddr *)&sin, &len) == -1)
	{
		closesocket(lsn_sock);
		return -1;
	}

	//port = ntohs(sin.sin_port);
	//cout <<	GetCurrentProcessId() << endl;
	//cout << "port = " << port << endl;

	if (getsockname(ctrl_sock, (struct sockaddr *)&sin, &len) == -1)
	{
		closesocket(lsn_sock);
		return -1;
	}

	sprintf(cmd, "PORT %d,%d,%d,%d,%d,%d\r\n",
		sin.sin_addr.s_addr & 0x000000FF,
		(sin.sin_addr.s_addr & 0x0000FF00) >> 8,
		(sin.sin_addr.s_addr & 0x00FF0000) >> 16,
		(sin.sin_addr.s_addr & 0xFF000000) >> 24,
		port >> 8, port & 0xff);

	if (execute_ftp_cmd(ctrl_sock, cmd, 1024, 200)) {
		closesocket(lsn_sock);
		return -1;
	}

	return lsn_sock;
}


extern "C" int iselect_mode(int lsnSocket)
{
	int readysocket;
	timeval tvTime;
	fd_set readfdset;
	sockaddr_in clnAddr;
	int clnLen;
	tvTime.tv_sec = 1;
	tvTime.tv_usec = 0;
	FD_ZERO(&readfdset);
	FD_SET(lsnSocket, &readfdset);

	unsigned long ulMode = 1;
	int ret = ioctlsocket(lsnSocket, FIONBIO, (unsigned long *)&ulMode);//设置成非阻塞模式。  
	if (ret == SOCKET_ERROR)//设置失败。 
	{
		return -1;
	}

	readfdset;
	int iret;
	if ((iret = select(lsnSocket + 1, &readfdset, NULL, NULL, &tvTime)) < 0){
		return -1;
	}

	if (iret > 0){
		if (FD_ISSET(lsnSocket, &readfdset)){
			clnLen = sizeof(clnAddr);
			if ((readysocket = accept(lsnSocket, (struct sockaddr*)&clnAddr, &clnLen)) < 0){
				return -1;
			}
		}
		return readysocket;
	}
	else{
		return 0;
	}
}

// UTF8编码转换到GBK编码
extern "C" int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpUTF8Str)  //如果UTF8字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, lpUnicodeStr, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //获取转换到GBK编码后所需要的字符空间长度

	if (!lpGBKStr)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nGBKStrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL);  //转换到GBK编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}

//GBK编码转换到UTF8编码
extern "C" int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpGBKStr)  //如果GBK字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, lpUnicodeStr, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, NULL, 0, NULL, NULL);  //获取转换到UTF8编码后所需要的字符空间长度

	if (!lpUTF8Str)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nUTF8StrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, (char *)lpUTF8Str, nUTF8StrLen, NULL, NULL);  //转换到UTF8编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}

