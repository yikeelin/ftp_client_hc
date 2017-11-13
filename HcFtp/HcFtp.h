#pragma once;  

//�ú������dll��Ŀ�ڲ�ʹ��__declspec(dllexport)����  
//��dll��Ŀ�ⲿʹ��ʱ����__declspec(dllimport)����  
//��DLL_IMPLEMENT��SimpleDLL.cpp�ж���  
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
	char listFileName[128]; //���ڼ��Ŀ���ļ��Ƿ����
	char dataBuf[DATABUFLEN];
	char currentDirectory[512];
}FTP_TRANSACTION;

char* list(char*, char*, char*);

extern "C" DLL_API int logon(char* serverIP, char* userName, char* passWord,
	char* lpErrorBuf, int iErrorBufLen);

/**

*@brief: �����ϴ��ļ���ftp������

*@author: Ljh

*@Parameters:
	char*	serverIP: ������IP��ַ
	char*	userName: Ftp�û���
	char*	passWord: Ftp����
	char*	serverFilePath: ������·��
	char*	clientFilePath: �ͻ���·��
	char*	fileName:		�ļ���
	char*	lpErrorBuf:		���ڱ��������Ϣ���ַ���
	int		iErrorBufLen	����
	char*	lpMD5Buf		���ڱ��淵�ص��ļ�md5��
	int		iMD5BufLen		����
	int		mode			����������ģʽ(0:���� 1:����)
*@exception: 

*@return: �ɹ�:0 ʧ��:-1(������Ϣ������lpErrorBuf��) -2(���ϴε���ʱ�����,�˿�δ��ȫ�ͷ���Ҫ����)
*@warning:

*/
extern "C" DLL_API int upload(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath, char* fileName,
	char* lpErrorBuf, int iErrorBufLen, char* lpMD5Buf, int iMD5BufLen, int mode);

/**

*@brief: ���ڴ�FTP�����������ļ�

*@author: Ljh

*@Parameters:
char*	serverIP: ������IP��ַ
char*	userName: Ftp�û���
char*	passWord: Ftp����
char*	serverFilePath: ������·��
char*	clientFilePath: �ͻ���·��
char*	fileName:		�ļ���
char*	lpErrorBuf:		���ڱ��������Ϣ���ַ���
int		iErrorBufLen	����
char*	MD5Str			Ҫ�����ļ���MD5��,����У�������ļ���������
int		mode			����������ģʽ(0:���� 1:����)
*@exception:

*@return: �ɹ�:0 ʧ��:-1(������Ϣ������lpErrorBuf��) -2(���ϴε���ʱ�����,�˿�δ��ȫ�ͷ���Ҫ����)
*@warning:

*/
extern "C" DLL_API int download(char* serverIP, char* userName, char* passWord,
	char* serverFilePath, char* clientFilePath,
	char* fileName, char* lpErrorBuf, int iErrorBufLen, char* MD5Str, int mode);

//��ȡ�ļ���С
extern "C" DLL_API int get_file_size(char* fileName, char* lpFileSizeBuf, int iFileSizeBufLen,
	char* lpErrorBuf, int iErrorBufLen);

//���ӷ�����
extern "C" int ftp_connect(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//�Ͽ�������
extern "C" int ftp_quit(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�ı䵱ǰĿ¼
extern "C" int ftp_cwd(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�ص���һ��Ŀ¼
extern "C" int ftp_cd_up(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//����Ŀ¼
extern "C" int ftp_mkd(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�б�
extern "C" char* ftp_list(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�����ļ�
extern "C" long long int ftp_retrfile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�ϴ��ļ�
extern "C" int ftp_storfile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//�޸��ļ���&�ƶ�Ŀ¼
extern "C" int ftp_renamefile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//ɾ���ļ�
extern "C" int ftp_deletefile(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);
//ɾ��Ŀ¼
extern "C" int ftp_deletefolder(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//�ı䴫��ģʽ type a type i
extern "C" int ftp_type(char mode, char* lpErrorBuf, int iErrorBufLen);

//ʹ���������뱻��ģʽ
extern "C" int ftp_entering_passive_mode(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//ʹ��������������ģʽ
extern "C" int ftp_entering_active_mode(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//�������ݶ˿�
extern "C" int ftp_connet_datasocket(FTP_TRANSACTION *ftpTransaction, char* lpErrorBuf, int iErrorBufLen);

//�ͷ���Դ
extern "C" void ftp_free_resource();



//�ӷ�����Ϣ��227 Entering Passive Mode (182,18,8,37,10,25).����  
//��ȡ���ݶ˿�  
extern "C" int get_portnum(char* msgbuf);

//ͨ������socketִ��FTP����  
extern "C" int execute_ftp_cmd(SOCKET controlSocket, char* msgbuf, int len, int stateCode);

//�ӷ�����Ϣ�л�ȡ״̬��  
extern "C" int get_statecode(char* msgbuf);

//����ģʽʱ����datasock�������������������
extern "C" int create_listensock_send2svr(int ctrl_sock);

extern "C" int iselect_mode(int lsnSocket);

extern "C" int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen);

extern "C" int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen);