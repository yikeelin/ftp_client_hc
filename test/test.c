#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")



/**

*@brief: �����ϴ��ļ���ftp������

*@author: Ljh

*@Parameters:
*int activeMode: ���������õ�ģʽ(0,����;1,����)
*string serviceIP: ������IP��ַ
*string userName: Ftp�û���
*string passWord: Ftp����
*string destinationDir: Ŀ��Ŀ¼
*string localFilePath: �����ļ�·��

*@exception:

*@return:
*string :��ʽ:"result:succeed|message:�ϴ��ɹ�" �� :"result:failed|message:�ϴ�ʧ��,�����ļ�������,Ŀ��·��������,�û����������..."

*@warning:

*/
char* upload(int activeMode, char* serviceIP, char* userName, char* passWord, char* destinationDir, char* localFilePath)
{

}


/**

*@brief: �����ϴ��ļ���ftp������

*@author: Ljh

*@Parameters:
*int activeMode: ���������õ�ģʽ(0,����;1,����)
*string serviceIP: ������IP��ַ
*string userName: Ftp�û���
*string passWord: Ftp����
*string destinationFilePath: Ŀ���ļ�·��
*string localFilePath: ���ش��Ŀ¼

*@exception:

*@return:
*string :��ʽ:"result:succeed|message:���سɹ�" �� :"result:failed|message:����ʧ��,Ŀ���ļ�������,�û����������..."

*@warning:

*/
char* download(int activeode, char*serviceIP, char* userName, char* passWord, char* destinationFilePath, char* localDir)
{
	
}



