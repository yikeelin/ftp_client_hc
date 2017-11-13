#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "hcftp.h"
#include "md5.h"

using namespace std;

void xiazai();
void shangchuang();

void main()
{

	xiazai();
	//shangchuang();
	//s = upload("127.0.0.1", "ljh", "123", "/", "e:", "qt.exe", "qt.exe");
	//s = download("127.0.0.1", "ljh", "123", "/b/ba", "e:", "qt-opensource-windows-x86-mingw530-5.7.1.exe", "qt-opensource-windows-x86-mingw530-5.7.1.exe");
	//s = logon("127.0.0.1", "ljh", "123");
	//s = getfilesizefromsvr("qt-opensource-windows-x86-mingw530-5.7.1.exe");

	//s = list("192.168.33.102", "sxnx", "1111");
	system("pause");
}

void xiazai()
{
	int iret;
	char lpErrorBuf[512];
	char md5retrun[33];
	char md5[33] = "C5BC945F8A4C007CD21CD2B1C1BA34EF";
	if (!(iret=download("192.168.55.97", "ljh", "123", "/", "e:/", "abc.doc", lpErrorBuf, 512, md5, 1)))
		//if (!download("192.168.188.196", "ssbwork", "123456", "/home/ssbwork", "e:/", "abc.doc", lpErrorBuf, 512, md5, 1))
		
	{
		cout << iret << endl;
		cout << "下载完成  Md5校验成功 = " << md5 << endl;
	}
	else
	{
		cout << iret << endl;
		cout << lpErrorBuf << endl;
	}
}

void shangchuang()
{
	char lpErrorBuf[512];
	char md5retrun[33];
	int iret;
	if (!(iret=upload("127.0.0.1", "ljh", "123", "/", "e:/", "hb.zip", lpErrorBuf, 512, md5retrun, 33, 1)))
	{
		cout << iret << endl;
		cout << "上传完成成功  Md5 = " << md5retrun << endl;
	}
	else
	{
		cout << iret << endl;
		cout << lpErrorBuf << endl;
	}
}
//void main1()
//{
//	FILE *pf = fopen("e:/xx.txt","rb+");
//	if (!pf)
//	{
//		cout << "pf = null" << endl;
//
//	}
//	else
//	{
//		int i = fseek(pf, 0, SEEK_END);
//		int index = ftell(pf);
//		cout << i << endl << index;
//		fwrite("\r\n666", 1, 5, pf);
//		fclose(pf);
//	}
//	system("pause");
//}
//
//void main()
//{
//	FILE *f = fopen("d:/hh.txt", "rb");
//	int i;
//	char buffer[128];
//	/*
//	getStringMD5("hello world", buffer);
//	printf("%s\n", buffer);*/
//	i = getFileMD5("d:/hh.txt", buffer);
//	printf("%s\n", buffer);
//	cout << i << endl << buffer << endl;
//	system("pause");
//}

