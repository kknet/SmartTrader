// fxcmApiTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SessionStatusListener.h"
#include "ResponseListener.h"
#include <ATLComTime.h>
#include "boost/thread.hpp"

COleDateTime String2Date(char const *  timeStr)
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

class threadTest
{
public:
	IO2GSession * pSession = NULL;
	SessionStatusListener *pSessionStatusListener = NULL;
	ResponseListener *pResponseListener = NULL;

	string  UserName;	//
	string  PWD;		//
	string  URL;		//"http://www.fxcorporate.com/Hosts.jsp"
	string  CONN;		// "Demo" or "Real".
	threadTest()
	{
		UserName = "701037785";
		PWD = "4616";
		URL = "http://www.fxcorporate.com/Hosts.jsp";
		CONN = "demo";
		pSession = CO2GTransport::createSession();
		pSessionStatusListener = new SessionStatusListener(
			pSession,
			UserName.c_str(),
			PWD.c_str());
		pSession->subscribeSessionStatus(pSessionStatusListener);

		pResponseListener = new ResponseListener(pSession);
		pSession->subscribeResponse(pResponseListener);
	}
	
protected:
private:

};

void run()
{
	IO2GSession * pSession = NULL;
	SessionStatusListener *pSessionStatusListener = NULL;
	ResponseListener *pResponseListener = NULL;

	string  UserName;	//
	string  PWD;		//
	string  URL;		//"http://www.fxcorporate.com/Hosts.jsp"
	string  CONN;		// "Demo" or "Real".

	UserName = "701037785";
	PWD = "4616";
	URL = "http://www.fxcorporate.com/Hosts.jsp";
	CONN = "demo";
	pSession = CO2GTransport::createSession();
	pSessionStatusListener = new SessionStatusListener(
		pSession,
		UserName.c_str(),
		PWD.c_str());
	pSession->subscribeSessionStatus(pSessionStatusListener);

	pResponseListener = new ResponseListener(pSession);
	pSession->subscribeResponse(pResponseListener);

	//
	if (!pSessionStatusListener->isConnected())
	{
		pSession->login(UserName.c_str(),
			PWD.c_str(),
			URL.c_str(),
			CONN.c_str());
		pSessionStatusListener->waitEvents();
		if (pSessionStatusListener->isConnected())
		{
			std::cout << "Login successed" << std::endl;
		}
		else
		{
			std::cout << "Login failed" << std::endl;
		}
	}
	
	std::cout << pSession->getSessionStatus() << std::endl;

	string sbeginDate = "2018-09-14 23:01:17";
	DATE beginDate = double(String2Date(sbeginDate.c_str()));
	string sEndDate = "2018-09-15 23:01:17";
	DATE endDate = double(String2Date(sEndDate.c_str()));
	string stimeFrame = "m1";
	string instrument = "EUR/USD";
	int maxBars = 1;

	O2G2Ptr<IO2GRequestFactory> factory = pSession->getRequestFactory();
	if (!factory)
	{
		std::cout << "Cannot create request factory" << std::endl;
		return;
	}

	O2G2Ptr<IO2GTimeframeCollection> timeframeCollection = factory->getTimeFrameCollection();
	O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get(stimeFrame.c_str());
	O2G2Ptr<IO2GRequest> request = factory->createMarketDataSnapshotRequestInstrument(
		instrument.c_str(),
		timeFrame,
		maxBars);
	if (!request)
	{
		std::cout << pSessionStatusListener->isConnected() << std::endl;
		std::cout << "Cannot create request : "
			<< instrument.c_str() << " "
			<< timeFrame << " "
			<< maxBars << " "
			<< __LINE__ << std::endl;
		return;
	}

}

int main()
{
	std::cout << "come in main" << std::endl;
	string AccountId = "701037785";
	string Password = "4616";
	string url = "http://www.fxcorporate.com/Hosts.jsp";
	//The name of the connection, for example "Demo" or "Real".
	string connection = "Demo";

	//boost::thread thred(run);
	run();

	while (true)
	{
		//std::cout << "run while" << std::endl;
	}

	string temp;
	std::cin >> temp ;
    return 0;
}

