
#include <iostream>  
#include <fstream>  
#include <WinSock2.h>  
#include <string>
#pragma comment(lib,"WS2_32.lib")  
using namespace std;

#define PORT 21     //FTP�˿�  
#define IP_ADDR "192.168.33.102"   //������ַ  
#define USER "sxnx"
#define PWD "1111"
#define BUFLEN 1024

int getPortNum(char* buf);
bool executeFTPCmd(SOCKET controlSocket, char* buf, int len, int stateCode);
int getStateCode(char* buf);

//int main()
//{
//	char buf[100];
//	char s[100];
//	memset(s, 0, 100);
//	gets(s);
//	memset(buf, 0, 100);
//	sprintf(buf, s, strlen(buf));
//	cout << strlen(buf) << endl;
//	sprintf(buf+strlen(buf), "\r\n", 4);
//	cout << strlen(buf) << endl;
//	cout << buf;
//	system("pause");
//}

int main()
{
	WSADATA dat;
	SOCKET controlSocket, dataSocket;
	SOCKADDR_IN serverAddr;
	int dataPort, ret, stateCode;
	char buf[BUFLEN] = { 0 }, sendBuf[BUFLEN] = { 0 };

	//��ʼ��������Ҫ  
	if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)  //Windows Sockets Asynchronous����  
	{
		cout << "Init Falied: " << GetLastError() << endl;
		system("pause");
		return -1;
	}

	//����Socket  
	controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (controlSocket == INVALID_SOCKET)
	{
		cout << "Creating Control Socket Failed: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	//�������������ʲ����ṹ��  
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDR); //��ַ  
	serverAddr.sin_port = htons(PORT);            //�˿�  
	memset(serverAddr.sin_zero, 0, sizeof(serverAddr.sin_zero));
	//����  
	ret = connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "Control Socket connecting Failed: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	cout << "Control Socket connecting is success." << endl;
	//���շ���״̬��Ϣ  
	recv(controlSocket, buf, BUFLEN, 0);
	cout << buf;                                                    //220  
	//���ݷ�����Ϣ��ȡ״̬�룬�����ж�  
	if (getStateCode(buf) != 220)
	{
		cout << "Error: Control Socket connecting ???" << endl;
		system("pause");
		return -1;
	}
	//�û���  
	//memset(buf, 0, BUFLEN);
	//sprintf(buf, "USER %s\r\n", USER);
	//if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 331)))                //331  
	//{
	//	cout << "Error: Send USERNAME Failed" << endl;
	//	system("pause");
	//	return -1;
	//}


	//����  
	//memset(buf, 0, BUFLEN);
	//sprintf(buf, "PASS %s\r\n", PWD);
	//if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 230)))                //230
	//{
	//	cout << "Error: Verify PASSWORD Failed" << endl;
	//	system("pause");
	//	return -1;
	//}

	while (true)
	{
		char s[100];
		memset(s, 0, 100);
		gets(s);
		memset(buf, 0, BUFLEN);
		sprintf(buf, s, strlen(s));
		//cout << strlen(s)<<endl;
		sprintf(buf+strlen(buf), "\r\n",2);
		//cout << buf <<endl;
		//cout << strlen(buf)<<endl;
		//char *x = buf;
		
		send(controlSocket, buf, strlen(buf), 0);
		memset(buf, 0, BUFLEN);
		int irec;
		Sleep(100);
		irec=recv(controlSocket, buf, 512, 0);
		cout << irec << endl;
		buf[irec] = 0;
		cout << buf;
	}

}


int main1()
{
	WSADATA dat;
	SOCKET controlSocket, dataSocket;
	SOCKADDR_IN serverAddr;
	int dataPort, ret, stateCode;
	char buf[BUFLEN] = { 0 }, sendBuf[BUFLEN] = { 0 };

	//��ʼ��������Ҫ  
	if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)  //Windows Sockets Asynchronous����  
	{
		cout << "Init Falied: " << GetLastError() << endl;
		system("pause");
		return -1;
	}

	//����Socket  
	controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (controlSocket == INVALID_SOCKET)
	{
		cout << "Creating Control Socket Failed: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	//�������������ʲ����ṹ��  
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDR); //��ַ  
	serverAddr.sin_port = htons(PORT);            //�˿�  
	memset(serverAddr.sin_zero, 0, sizeof(serverAddr.sin_zero));
	//����  
	ret = connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "Control Socket connecting Failed: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	cout << "Control Socket connecting is success." << endl;
	//���շ���״̬��Ϣ  
	recv(controlSocket, buf, BUFLEN, 0);
	cout << buf;                                                    //220  
	//���ݷ�����Ϣ��ȡ״̬�룬�����ж�  
	if (getStateCode(buf) != 220)
	{
		cout << "Error: Control Socket connecting ???" << endl;
		system("pause");
		return -1;
	}
	//�û���  
	memset(buf, 0, BUFLEN);
	sprintf(buf, "USER %s\r\n", USER);
	if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 331)))                //331  
	{
		cout << "Error: Send USERNAME Failed" << endl;
		system("pause");
		return -1;
	}
	

	//����  
	memset(buf, 0, BUFLEN);
	sprintf(buf, "PASS %s\r\n", PWD);
	if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 230)))                //230
	{
		cout << "Error: Verify PASSWORD Failed" << endl;
		system("pause");
		return -1;
	}
	
	//=======================================  
	//�л�������ģʽ  
	memset(buf, 0, BUFLEN);
	sprintf(buf, "PASV\r\n");
	if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 227)))                //227
	{
		cout << "error: entering passive mode failed !227" << endl;
		system("pause");
		return -1;
	}

	//�л�����ģʽ
	//memset(buf, 0, BUFLEN);
	//sprintf(buf, "PORT 127,0,0,1,27,233\r\n");
	//if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 200)))                //227
	//{
	//	cout << "Error: Entering PORT Mode Failed !" << endl;
	//	system("pause");
	//	exit(-1);
	//}
	//system("pause");

	//���ص���Ϣ��ʽΪ---h1,h2,h3,h4,p1,p2  
	//����h1,h2,h3,h4Ϊ�������ĵ�ַ��p1*256+p2Ϊ���ݶ˿�  
	dataPort = getPortNum(buf);
	//�ͻ������ݴ���socket  
	dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverAddr.sin_port = htons(dataPort);    //�������Ӳ����е�portֵ  
	ret = connect(dataSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "Data Socket connecting Failed: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	cout << "Data Socket connecting is success." << endl;


	//���ĵ�ǰĿ¼  
	memset(buf, 0, BUFLEN);
	sprintf(buf, "CWD %s\r\n", "/a");   //250  
	if (!(executeFTPCmd(controlSocket, buf, BUFLEN, 250)))                //250
	{
		cout << "error: No such file or directory!550" << endl;
		system("pause");
		return -1;
	}
	cout << "250 Directory changed";
	

	//�ϴ��ļ�  
	memset(buf, 0, BUFLEN);
	sprintf(buf, "STOR %s\r\n", "hh.txt");
	executeFTPCmd(controlSocket, buf, BUFLEN, 150);                        //150 
	//550 Permission denied. Ȩ�޲���

	FILE* f = fopen("d:/abc.txt", "rb");
	if (f == NULL)
	{
		cout << "The file pointer is NULL!" << endl;
	//	cout << "Error: " << __FILE__ << " " << __LINE__ << endl;
		system("pause");
		exit(-1);
	}
	while (!feof(f))
	{
		fread(sendBuf, 1, 1024, f);
		send(dataSocket, sendBuf, 1024, 0);
	}
	cout << "finish";
	fclose(f);
	//�ͷ���Դ  
	closesocket(dataSocket);
	closesocket(controlSocket);
	WSACleanup();
	system("pause");
	return 0;
}

//�ӷ�����Ϣ��227 Entering Passive Mode (182,18,8,37,10,25).����  
//��ȡ���ݶ˿�  
int getPortNum(char* buf)
{
	int num1 = 0, num2 = 0;

	char* p = buf;
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
	cout << "The data port number is " << num1 * 256 + num2 << endl;
	return num1 * 256 + num2;
}
//ͨ������socketִ��FTP����  
bool executeFTPCmd(SOCKET controlSocket, char* buf, int len, int stateCode)
{
	send(controlSocket, buf, strlen(buf), 0);
	memset(buf, 0, len);
	recv(controlSocket, buf, BUFLEN, 0);
	cout << buf;
	if (getStateCode(buf) == stateCode)
	{
		return true;
	}
	else
	{
		cout << "The StateCode is Error!" << endl;
		return false;
	}
}
//�ӷ�����Ϣ�л�ȡ״̬��  
int getStateCode(char* buf)
{
	int num = 0;
	char* p = buf;
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