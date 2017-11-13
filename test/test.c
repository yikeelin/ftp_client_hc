#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")



/**

*@brief: 用于上传文件到ftp服务器

*@author: Ljh

*@Parameters:
*int activeMode: 服务器采用的模式(0,被动;1,主动)
*string serviceIP: 服务器IP地址
*string userName: Ftp用户名
*string passWord: Ftp密码
*string destinationDir: 目标目录
*string localFilePath: 本地文件路径

*@exception:

*@return:
*string :格式:"result:succeed|message:上传成功" 或 :"result:failed|message:上传失败,本地文件不存在,目标路径不存在,用户名密码错误..."

*@warning:

*/
char* upload(int activeMode, char* serviceIP, char* userName, char* passWord, char* destinationDir, char* localFilePath)
{

}


/**

*@brief: 用于上传文件到ftp服务器

*@author: Ljh

*@Parameters:
*int activeMode: 服务器采用的模式(0,被动;1,主动)
*string serviceIP: 服务器IP地址
*string userName: Ftp用户名
*string passWord: Ftp密码
*string destinationFilePath: 目标文件路径
*string localFilePath: 本地存放目录

*@exception:

*@return:
*string :格式:"result:succeed|message:下载成功" 或 :"result:failed|message:下载失败,目标文件不存在,用户名密码错误..."

*@warning:

*/
char* download(int activeode, char*serviceIP, char* userName, char* passWord, char* destinationFilePath, char* localDir)
{
	
}



