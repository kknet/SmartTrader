#pragma once
#include "stdafx.h"
#include <string>
#include <ATLComTime.h>
using namespace std;
using namespace boost::python;
/*
static修饰的类成为静态类，静态类中只能包含静态成员（被static修饰的字段、属性、方法），
不能被实例化，不能被继承；非静态中可以包含静态成员
*/
 static class Tools
{
	//静态类中不能存在构造函数  
public:
	//static void formatDate(DATE date, char *buf);
	static void formatDate(DATE d, std::string &buf);
	//static COleDateTime myString2Date(string  timeStr);
	static COleDateTime String2Date(char const * timeStr);
	///-------------------------------------------------------------------------------------
	///从Python对象到C++类型转换用的函数
	///-------------------------------------------------------------------------------------
	//从字典中获取某个建值对应的整数，并赋值到请求结构体对象的值上
	static void getInt(boost::python::dict d, string key, int *value);
	//从字典中获取某个建值对应的浮点数，并赋值到请求结构体对象的值上
	static void getDouble(boost::python::dict d, string key, double *value);
	//从字典中获取某个建值对应的字符，并赋值到请求结构体对象的值上
	static  void getStr(dict d, string key, char *value);
	//从字典中获取某个建值对应的字符串，并赋值到请求结构体对象的值上
	static  void getChar(dict d, string key, char *value);
	//将时间从本地时间转换为UTC时间
	static DATE ConvertLocal2UTC(IO2GSession *pSession, DATE s);
	//将UTC时间转换为本地时间
	static DATE ConvertUTC2Local(IO2GSession *pSession, DATE s);

};

