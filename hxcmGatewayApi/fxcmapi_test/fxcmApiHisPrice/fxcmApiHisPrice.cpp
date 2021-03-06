// fxcmApiHisPrice.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SessionStatusListener.h"
#include "ResponseListener.h"
#include <ATLComTime.h>
#include "boost/thread.hpp"
using namespace std;

string AccountId = "701037785";
string Password = "4616";
string url = "http://www.fxcorporate.com/Hosts.jsp";
//The name of the connection, for example "Demo" or "Real".
string connection = "Demo";
COleDateTime beginDate(2016, 5, 2, 0, 0, 0);
COleDateTime endDate(2016, 5, 3, 23, 0, 0);
void formatDate(DATE date, char *buf)
{
	struct tm tmBuf = { 0 };
	CO2GDateUtils::OleTimeToCTime(date, &tmBuf);

	using namespace std;
	stringstream sstream;
	sstream << setw(2) << setfill('0') << tmBuf.tm_mon + 1 << "." \
		<< setw(2) << setfill('0') << tmBuf.tm_mday << "." \
		<< setw(4) << tmBuf.tm_year + 1900 << " " \
		<< setw(2) << setfill('0') << tmBuf.tm_hour << ":" \
		<< setw(2) << setfill('0') << tmBuf.tm_min << ":" \
		<< setw(2) << setfill('0') << tmBuf.tm_sec;
	//strcpy(buf, sstream.str().c_str());
	int size = sizeof(sstream.str().c_str());
	int isize = strlen(sstream.str().c_str()) + 1;
	strcpy_s(buf, isize, sstream.str().c_str());
}
void printPrices(IO2GSession *session, IO2GResponse *response)
{
	if (response != 0)
	{
		if (response->getType() == MarketDataSnapshot)
		{
			std::cout << "Request with RequestID='" << response->getRequestID() << "' is completed:" << std::endl;
			O2G2Ptr<IO2GResponseReaderFactory> factory = session->getResponseReaderFactory();
			if (factory)
			{
				O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = factory->createMarketDataSnapshotReader(response);
				if (reader)
				{
					char sTime[20];
					for (int ii = reader->size() - 1; ii >= 0; ii--)
					{
						DATE dt = reader->getDate(ii);
						formatDate(dt, sTime);
						if (reader->isBar())
						{
							printf("DateTime=%s, BidOpen=%f, BidHigh=%f, BidLow=%f, BidClose=%f, AskOpen=%f, AskHigh=%f, AskLow=%f, AskClose=%f, Volume=%i\n",
								sTime, reader->getBidOpen(ii), reader->getBidHigh(ii), reader->getBidLow(ii), reader->getBidClose(ii),
								reader->getAskOpen(ii), reader->getAskHigh(ii), reader->getAskLow(ii), reader->getAskClose(ii), reader->getVolume(ii));
						}
						else
						{
							printf("DateTime=%s, Bid=%f, Ask=%f\n", sTime, reader->getBid(ii), reader->getAsk(ii));
						}
					}
				}
			}
		}
	}
}

void run()
{
	IO2GSession *pSession = CO2GTransport::createSession();
	SessionStatusListener *pSessionStatusListener = new SessionStatusListener(pSession, true, AccountId.c_str(), Password.c_str());
	pSession->subscribeSessionStatus(pSessionStatusListener);

	// 2. Create a response listener object
	ResponseListener * responseListener = new ResponseListener(pSession);
	pSession->subscribeResponse(responseListener);
	// 3. Login
	pSession->login(AccountId.c_str(), Password.c_str(), url.c_str(), connection.c_str());

	if (pSessionStatusListener->waitEvents() && pSessionStatusListener->isConnected())
	{
		// 4. Create IO2GRequestFactory:
		O2G2Ptr<IO2GRequestFactory> factory = pSession->getRequestFactory();
		if (!factory)
		{
			std::cout << "Cannot create request factory" << std::endl;
			return ;
		}
		O2G2Ptr<IO2GTimeframeCollection> timeframeCollection = factory->getTimeFrameCollection();
		O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get("m1");

		//
		// 7. Create a market data snapshot request (IO2GRequest) 
		//using the instrument, time frame, and maximum number of bars as arguments:
		//O2G2Ptr<IO2GRequest> * getMarketDataSnapshot = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);

		O2G2Ptr<IO2GRequest> request = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);
		try
		{
			factory->fillMarketDataSnapshotRequestTime(request, 0, 0, false);
			responseListener->setRequestID(request->getRequestID());
			pSession->sendRequest(request);
			std::cout <<  request->getRequestID() << std::endl;
		}
		catch (const std::exception&ee)
		{
			std::cout << ee.what() << std::endl;
		}
		if (!request)
		{
			pSession->sendRequest(request);
		}

	}
	pSession->unsubscribeSessionStatus(pSessionStatusListener);
	pSessionStatusListener->release();
	pSession->release();
	
}

int main()
{
	std::cout <<"COleDateTime beginDate =" << float(beginDate) << std::endl;
	//std::cout << "COleDateTime() =" << COleDateTime( << std::endl;
	double tempTime = 42664.5;
	char sTime[20];
	std::cout << tempTime << std::endl;
	formatDate(tempTime, sTime);
	std::cout << sTime << std::endl;
	bool bWasError = false;
	/*
	// 1. Create a session, a session status listener, subscribe the listener to the session status
	IO2GSession *pSession = CO2GTransport::createSession();
	SessionStatusListener *pSessionStatusListener = new SessionStatusListener(pSession,true, AccountId.c_str(), Password.c_str());
	pSession->subscribeSessionStatus(pSessionStatusListener);

	// 2. Create a response listener object
	ResponseListener * responseListener = new ResponseListener(pSession);
	pSession->subscribeResponse(responseListener);
	// 3. Login
	pSession->login(AccountId.c_str(), Password.c_str(), url.c_str(), connection.c_str());
	/*
	if ( pSessionStatusListener->waitEvents() && pSessionStatusListener->isConnected() )
	{
		std::cout << "begin to qry prices" << std::endl;
		// 4. Create IO2GRequestFactory:
		O2G2Ptr<IO2GRequestFactory> factory = pSession->getRequestFactory();
		if (!factory)
		{
			std::cout << "Cannot create request factory" << std::endl;
			return false;
		}
		//5. Get IO2GTimeframeCollection:
		O2G2Ptr<IO2GTimeframeCollection> timeframeCollection = factory->getTimeFrameCollection();
		int isize = timeframeCollection->size();
		for (int i = 0; i < timeframeCollection->size(); i++)
		{
			O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get(i);
			int QueryDepth = timeFrame->getQueryDepth();
			int Size = timeFrame->getSize();
			O2GTimeframeUnit unit = timeFrame->getUnit();
			std::cout <<"QueryDepth = "<< QueryDepth <<","<<" unit ="<< unit <<std::endl;
		}

		//IO2GTimeframeCollection * timeFrames = factory->getTimeFrameCollection();

		// 6. Get the IO2GTimeframe you need:
		O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get(0);
		//IO2GTimeframe * timeFrame = timeFrames->get("m1");

		if (!timeFrame)
		{
			std::cout << "Timeframe '" << "type" << "' is incorrect!" << std::endl;
			return false;
		}
		// 7. Create a market data snapshot request (IO2GRequest) 
		//using the instrument, time frame, and maximum number of bars as arguments:
		//O2G2Ptr<IO2GRequest> * getMarketDataSnapshot = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);
		
		O2G2Ptr<IO2GRequest> request = factory->createMarketDataSnapshotRequestInstrument("EUR/USD", timeFrame, 300);
		// 8.Fill the market data snapshot request 
		//using the request, date and time "from", and date and time "to" as arguments
		int count = 0;
		do
		{
			//factory->fillMarketDataSnapshotRequestTime(request, (0.0), (0.0), false);
			std::cout << timeFrame << std::endl;
			//factory->fillMarketDataSnapshotRequestTime(request, beginDate, endDate, false);
			factory->fillMarketDataSnapshotRequestTime(request, 0, 0, false);
			responseListener->setRequestID(request->getRequestID());
			pSession->sendRequest(request);

			//阻塞在这里等事件
			if (!responseListener->waitEvents())
			{
				std::cout << "Response waiting timeout expired" << std::endl;
				return false;
			}
			O2G2Ptr<IO2GResponse> response = responseListener->getResponse();
			// 9. If the type of response is MarketDataSnapshot, 
			//process the response to extract necessary information
			if (response && response->getType() == MarketDataSnapshot)
			{
				// 10. Get IO2GResponseReaderFactory:
				O2G2Ptr<IO2GResponseReaderFactory> readerFactory = pSession->getResponseReaderFactory();
				if (readerFactory)
				{
					// 11. Create IO2GMarketDataSnapshotResponseReader:
					O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = readerFactory->createMarketDataSnapshotReader(response);
					if (reader->size() > 0)
					{
						std::cout << reader->size() <<" rows received" << std::endl;
						float a = float(endDate);
						float b = reader->getDate(0);
						if (fabs(float(endDate) - reader->getDate(0)) > 0.0001)
							endDate = reader->getDate(0); // earliest datetime of returned data
						else
							break;
					}
					else
					{
						std::cout << "\n 0 rows received" << std::endl;
						break;
					}
				}
				printPrices(pSession, response);
			}

		} while (float(endDate) - float(beginDate) > 0.0001);

		
	}
	else
	{
		bWasError = true;
	}
	
	if (pSessionStatusListener->waitEvents() && pSessionStatusListener->isConnected())
	{
		IO2GTimeConverter *timeConverter = pSession->getTimeConverter();
		for (int i = 0; i < 1; i++)
		{
			std::cout << "SessionStatus = " <<pSession->getSessionStatus() << std::endl;
			std::cout <<" pSession->getServerTime = " <<pSession->getServerTime() << std::endl;
			COleDateTime beginDate(2018, 10, 1, 12, 0, 0);
			double tempTime = beginDate;
			char sTime[20];
			DATE d = timeConverter->convert(tempTime, IO2GTimeConverter::Local, IO2GTimeConverter::UTC);
			formatDate(d, sTime);
			std::cout << sTime << std::endl;

			d = timeConverter->convert(tempTime, IO2GTimeConverter::Local, IO2GTimeConverter::Local);
			formatDate(d, sTime);
			std::cout << sTime << std::endl;

			d = timeConverter->convert(tempTime, IO2GTimeConverter::Local, IO2GTimeConverter::EST);
			formatDate(d, sTime);
			std::cout << sTime << std::endl;

			d = timeConverter->convert(tempTime, IO2GTimeConverter::Local, IO2GTimeConverter::Server);
			formatDate(d, sTime);
			std::cout << sTime << std::endl;
		}
		std::cout << pSession->getServerTime() << std::endl;
		timeConverter->release();
	}
	pSession->unsubscribeSessionStatus(pSessionStatusListener);
	pSessionStatusListener->release();
	pSession->release();
	//
	*/

	//run();
	boost::thread thred(run);
	while (1)
	{	
		;
	}
    return 0;
}
