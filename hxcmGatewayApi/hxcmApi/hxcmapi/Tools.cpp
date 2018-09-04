#include"stdafx.h"
#include "Tools.h"

using namespace std;
// ���������2018-08-30 15:35:34 369����֧��"/"
void Tools::formatDate(DATE d, std::string &buf)
{
	struct tm t;
	CO2GDateUtils::OleTimeToCTime(d, &t);
	std::stringstream sstream;
	sstream << std::setfill('0') << std::setw(4) << t.tm_year + 1900 << "-"
		<< std::setfill('0') << std::setw(2) << t.tm_mon + 1 << "-"
		<< std::setfill('0') << std::setw(2) << t.tm_mday << " "
		<< std::setfill('0') << std::setw(2) << t.tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << t.tm_min << ":"
		<< std::setfill('0') << std::setw(2) << t.tm_sec;
	buf = sstream.str();
}

// ����ʱ�䣺2018-08-30 15:35:34 369����֧��"/"
 COleDateTime Tools::String2Date(char const *  timeStr)
{
	struct tm stTm;
	sscanf_s(timeStr, "%d-%d-%d %d:%d:%d",
		&(stTm.tm_year),
		&(stTm.tm_mon),
		&(stTm.tm_mday),
		&(stTm.tm_hour),
		&(stTm.tm_min),
		&(stTm.tm_sec));

	//stTm.tm_year -= 1900;
	//stTm.tm_mon--;
	//stTm.tm_isdst = -1;
	return COleDateTime(stTm.tm_year, stTm.tm_mon, stTm.tm_mday, stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
}

 //��ʱ��ӱ���ʱ��ת��ΪUTCʱ��
 DATE Tools::ConvertLocal2UTC(IO2GSession *pSession,DATE s)
 {
	 IO2GTimeConverter *timeConverter = pSession->getTimeConverter();
	 DATE d = timeConverter->convert(s, IO2GTimeConverter::Local, IO2GTimeConverter::UTC);
	 timeConverter->release();
	 return d;
 }
 //��UTCʱ��ת��Ϊ����ʱ��
 DATE Tools::ConvertUTC2Local(IO2GSession *pSession,DATE s)
 {
	 IO2GTimeConverter *timeConverter = pSession->getTimeConverter();
	 DATE d = timeConverter->convert(s, IO2GTimeConverter::UTC, IO2GTimeConverter::Local);
	 timeConverter->release();
	 return d;
 }

 ///-------------------------------------------------------------------------------------
 ///��Python����C++����ת���õĺ���
 ///-------------------------------------------------------------------------------------


 void Tools::getInt(boost::python::dict d, string key, int *value)
 {
	 if (d.has_key(key))		//����ֵ����Ƿ���ڸü�ֵ
	 {
		 boost::python::api::object o = d[key];	//��ȡ�ü�ֵ
		 boost::python::extract<int> x(o);	//������ȡ��
		 if (x.check())		//���������ȡ
		 {
			 *value = x();	//��Ŀ������ָ�븳ֵ
		 }
	 }
 }

 void Tools::getDouble(boost::python::dict d, string key, double *value)
 {
	 if (d.has_key(key))
	 {
		 boost::python::api::object o = d[key];
		 boost::python::extract<double> x(o);
		 if (x.check())
		 {
			 *value = x();
		 }
	 }
 }
 void Tools::getStr(dict d, string key, char *value)
 {
	 if (d.has_key(key))
	 {
		 object o = d[key];
		 extract<string> x(o);
		 if (x.check())
		 {
			 string s = x();
			 const char *buffer = s.c_str();
			 //���ַ���ָ�븳ֵ����ʹ��strcpy_s, vs2013ʹ��strcpy����ͨ����
			 //+1Ӧ������ΪC++�ַ����Ľ�β���ţ������ر�ȷ�����������1�����
			 strcpy_s(value, strlen(buffer) + 1, buffer);

		 }
	 }
 }

 void Tools::getChar(dict d, string key, char *value)
 {
	 if (d.has_key(key))
	 {
		 object o = d[key];
		 extract<string> x(o);
		 if (x.check())
		 {
			 string s = x();
			 const char *buffer = s.c_str();
			 *value = *buffer;
		 }
	 }
 };