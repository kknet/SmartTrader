// tempTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SessionStatusListener.h"


int main()
{
	string AccountId = "701037785";
	string Password = "4616";
	string url = "http://www.fxcorporate.com/Hosts.jsp";
	//The name of the connection, for example "Demo" or "Real".
	string connection = "Demo";

	// 2 Create a session:
	IO2GSession *pSession = CO2GTransport::createSession();
	// 3 Create an instance of the session status listener:
	CSessionStatusListener *sessionStatusListener = new CSessionStatusListener(pSession, "701037785", "4616");
	// 4 bscribe the status listener object to the session status. 
	pSession->subscribeSessionStatus(sessionStatusListener);
	// 5 login
	try
	{
		pSession->login(AccountId.c_str(), Password.c_str(), url.c_str(), connection.c_str());
		if (sessionStatusListener->Connected)
		{
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	pSession->unsubscribeSessionStatus(sessionStatusListener);
	delete sessionStatusListener;


	string temp;
	std::cin >> temp;
	return 0;
}

