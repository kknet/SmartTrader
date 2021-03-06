// hxcmapi.cpp: 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "hxcmapi.h"
#include "Tools.h"
#include "SessionStatusListener.h"
#include "ResponseListener.h"
#include <ATLComTime.h>
#include <string>
#include "boost/timer/timer.hpp"
//#include <cstdlib>
#define PRINTLINE(msg) std::cout << msg << "  " <<"line no =" << __LINE__ << std::endl;
using namespace std;


HxcmApi::HxcmApi()
{
	function0<void> f = boost::bind(&HxcmApi::processTask, this);
	thread t(f);
	this->pTask_thread = &t;
	//
	pSession = NULL;
	pSessionStatusListener = NULL;
};

HxcmApi::HxcmApi(string  userName, string   pwd, string  url, string connection)
{
	//
	this->UserName = userName;
	this->PWD = pwd;
	this->URL = url;
	this->CONN = connection;

	//processTask在另一个线程中运行，提高效率
	function0<void> f = boost::bind(&HxcmApi::processTask, this);
	thread t(f);
	this->pTask_thread = &t;

	//创建会话
	this->pSession = CO2GTransport::createSession();

	// 创建会话的StatusListener
	this->pSessionStatusListener = new SessionStatusListener(
		pSession,
		userName.c_str(),
		pwd.c_str());
	this->pSession->subscribeSessionStatus(pSessionStatusListener);
	//this->pSessionStatusListener->api = this;

	// 创建会话的ResponseListener
	this->pResponseListener = new ResponseListener(this->pSession, this);
	this->pSession->subscribeResponse(pResponseListener);

	//
	std::list<string> h1;
	h1.push_back("USD/CHN");
	h1.push_back("USD/EUR");
	this->Tick_instruments["H1"] = h1;

	std::list<string> m1;
	//m1.push_back("USD/CHN");
	m1.push_back("EUR/USD");
	this->Tick_instruments["m1"] = m1;

}

//=========================================================================
//调试用函数
//=========================================================================
IO2GOfferRow *HxcmApi::getOfferAndPrint(IO2GSession *session, const char *sInstrument,bool IsPrint)
{
	if (!session || !sInstrument)
		return NULL;

	IO2GOfferRow *resultOffer = NULL;
	O2G2Ptr<IO2GLoginRules> loginRules = session->getLoginRules();
	if (loginRules)
	{
		O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(Offers);
		if (response)
		{
			O2G2Ptr<IO2GResponseReaderFactory> readerFactory = session->getResponseReaderFactory();
			if (readerFactory)
			{
				O2G2Ptr<IO2GOffersTableResponseReader> reader = readerFactory->createOffersTableReader(response);
				for (int i = 0; i < reader->size(); ++i)
				{
					O2G2Ptr<IO2GOfferRow> offer = reader->getRow(i);
					if (offer)
					{
						// 输出货币对的订阅状态
						if (IsPrint)
						{
							if (strcmp(offer->getSubscriptionStatus(), O2G2::SubscriptionStatuses::ViewOnly) == 0)
							{
								std::cout << offer->getInstrument() << "\t:\t[V]iew only" << std::endl;
							}
							else if (strcmp(offer->getSubscriptionStatus(), O2G2::SubscriptionStatuses::Disable) == 0)
							{
								std::cout << offer->getInstrument() << "\t:\t[D]isabled" << std::endl;
							}
							else if (strcmp(offer->getSubscriptionStatus(), O2G2::SubscriptionStatuses::Tradable) == 0)
							{
								std::cout << offer->getInstrument() << "\t:\t[T]rade" << std::endl;
							}
							else
							{
								std::cout << offer->getInstrument() << "\t:\t" << offer->getSubscriptionStatus() << std::endl;
							}
							//std::cout << std::flush;
						}																			
						if (strcmp(offer->getInstrument(), sInstrument) == 0)
						{
							resultOffer = offer.Detach();
						}
							
					}
				}
			}
		}
	}
	return resultOffer;
}

//=====================================================================================

void HxcmApi::printPrices(IO2GSession *session, IO2GResponse *response)
{
	if (response != 0)
	{
		if (response->getType() == MarketDataSnapshot)
		{
			std::cout << "Request with RequestID='" << response->getRequestID() << "' 已经完成:" << std::endl;
			O2G2Ptr<IO2GResponseReaderFactory> factory = session->getResponseReaderFactory();
			if (factory)
			{
				O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = factory->createMarketDataSnapshotReader(response);
				if (reader)
				{
					string sTime;
					for (int ii = reader->size() - 1; ii >= 0; ii--)
					{
						DATE dt = reader->getDate(ii);
						Tools::formatDate(dt, sTime);
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
// 发送消息给python客户端
void HxcmApi::sendMessage(string msg)
{
	//std::cout << "call HxcmApi::sendMessage" << __LINE__ << std::endl;
	Task task = Task();
	task.task_name = OnMessage_smart;
	task.task_data = "HxcmApi Message : " + msg;
	//不能是中文,
	//传出去之后字符串内容就消失了 ，struct 中不能用string吗！！
	//task.task_data = msg;
	//strcpy_s(task.message, strlen(msg.c_str()), msg.c_str());
	this->putTask(task);
}
//======================================================================================
//任务处理部函数，不输出到python中
//======================================================================================

void HxcmApi::putTask(Task task)
{
	this->task_queue.push(task);
}
void HxcmApi::processTask()
{
	while (1)
	{
		//取出队列中的数据
		Task task = this->task_queue.wait_and_pop();
		switch (task.task_name)
		{
			//历史数据
		case OnGetHisPrices_smart:
		{
			this->processGetHisPrices(task);
			break;
		}
		case OnGetSubScribeData_smart:
		{
			this->processGetSubScribeData(task);
			break;
		}			
		case OnMessage_smart:		
		{
			this->processMessage(task);
			break;
		}
		default:
			break;
		}
	}
}

//历史数据查询的respons数据处理
void HxcmApi::processGetHisPrices(Task task)
{
	//std::cout << "IN HxcmApi::processGetHisPrices" << __LINE__ << std::endl;
	PyLock lock;
	try
	{
		O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = any_cast<O2G2Ptr<IO2GMarketDataSnapshotResponseReader>>(task.task_data);
		boost::python::list rows;
		string sTime;
		for (int i = reader->size() - 1; i >= 0; i--)
		{
			boost::python::dict rowdata;
			if (reader->isBar())
			{
				//rowdata["instrument"] = reqData.instrument;
				DATE dt = reader->getDate(i);
				//将时间由UTC时间转换为本地时间
				DATE dtemp = Tools::ConvertUTC2Local(this->pSession, dt);
				Tools::formatDate(dtemp, sTime);
				rowdata["Date"] = sTime;
				rowdata["BidOpen"] = reader->getBidOpen(i);
				rowdata["BidHigh"] = reader->getBidHigh(i);
				rowdata["BidLow"] = reader->getBidLow(i);
				rowdata["BidClose"] = reader->getBidClose(i);
				rowdata["AskOpen"] = reader->getAskOpen(i);
				rowdata["AskHigh"] = reader->getAskHigh(i);
				rowdata["AskLow"] = reader->getAskLow(i);
				rowdata["AskClose"] = reader->getAskClose(i);
				rowdata["Volume"] = reader->getVolume(i);

			}
			else
			{
				//rowdata["instrument"] = reqData.instrument;
				rowdata["Bid"] = reader->getBid(i);
				rowdata["Ask"] = reader->getAsk(i);
			}
			rows.append(rowdata);

		}
		boost::python::dict data;
		data["instrument"] = "usd_cnh";
		data["data"] = rows;
		data["nums"] = reader->size();

		//this->onResGetHistoryPrices( (boost::python::dict) task.task_data, false);
		this->onResGetHistoryPrices(data, false);
	}
	catch (const std::exception& ee)
	{
		std::cout << ee.what() << __LINE__ << std::endl;
		return;
	}	
}

//货币对市场报价的订阅返回数据
void HxcmApi::processGetSubScribeData(Task task)
{
	PyLock lock;
	//1.数据类型转换	
	O2G2Ptr<IO2GTablesUpdatesReader> reader;
	try
	{
		reader = any_cast<O2G2Ptr<IO2GTablesUpdatesReader>>(task.task_data);
	}
	catch (const std::exception& ee)
	{
		std::cout << ee.what() << std::endl;
		return;
	}
	// 2. 从reader中遍历数据
	if (reader)
	{
		boost::python::list rows;
		if (reader->size() < 1)
		{
			return;
		}
		for (int i = 0; i < reader->size(); ++i)
		{
			if (reader->getUpdateType(i) == Update)
			{
				boost::python::list rowdata;
				O2G2Ptr<IO2GOfferRow> offerRow = reader->getOfferRow(i);
				rowdata.append(offerRow->getInstrument());
				rowdata.append(offerRow->getAsk());
				rowdata.append(offerRow->getBid());
				//
				rows.append(rowdata);
			}
		}
		// 3. 发送给事件处理函数
		this->onSubscribeInstrument(rows);
	}
}


//消息类任务的处理函数，将消息封装为python兼容数据类型
void HxcmApi::processMessage(Task task)
{
	PyLock lock;	
	try
	{
		boost::python::dict data;
		string temp = any_cast<string>(task.task_data);
		data["data"] = temp;
		//std::cout << temp << __LINE__ <<std::endl;
		this->onMessage(data);
	}
	catch(boost::bad_any_cast &e) {
		std::cout << e.what() << __LINE__ << std::endl;
		return;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << __LINE__ <<std::endl;
		return;
	}
}

///-------------------------------------------------------------------------------------
///回调函数，用于处理异步消息
///-------------------------------------------------------------------------------------
void HxcmApi::Login(bool IsBlock )
{

	if (!pSessionStatusListener->isConnected() )
	{
		//std::cout << "HxcmApi::Login() " << __LINE__ <<std::endl;
		pSession->login(UserName.c_str(), 
						PWD.c_str(), 
						URL.c_str(), 
						CONN.c_str());
		//首次登陆时应该等待登陆完成在返回，阻塞模式
		if (IsBlock == true)
		{
			if (pSessionStatusListener->waitEvents() && pSessionStatusListener->isConnected())
			{
				return;
			}
		}		
	}
}

void HxcmApi::Logout()
{
	//注销ResponseListener中的消息回调
	pSession->unsubscribeResponse(pResponseListener);
	pResponseListener->release();

	//注销SessionStatusListener中的消息回调
	pSession->unsubscribeSessionStatus(pSessionStatusListener);
	//重置被引用计数器
	pSessionStatusListener->reset();

	pSession->logout();

	//等待相应消息处理完
	//pSessionStatusListener->waitEvents();
	//pResponseListener->waitEvents();

}

//查询历史价格//
bool HxcmApi::qryHisPrices(string instrument, string stimeFrame, int maxBars,string sbeginDate,string sEndDate)
{
	std::cout << instrument << " " << stimeFrame << " " << sbeginDate << " " << sEndDate << "  " << __LINE__ << std::endl;

	// 处理fromDate，toDate为0的情况
	//如果beginDate和EndDate 等于0，即为当下
	//时间转换
	DATE beginDate = -1 ;
	DATE endDate = -1;
	try
	{
		beginDate = atof(sbeginDate.c_str());
		std::cout << beginDate <<" : "<< sbeginDate<<" " << __LINE__ << std::endl;
		if (beginDate != 0.0)
		{
			beginDate = double(Tools::String2Date(sbeginDate.c_str()));
			std::cout << "line no =" << __LINE__ << std::endl;
		}

		endDate = atof(sEndDate.c_str());
		//std::cout << endDate << " " << __LINE__ << std::endl;
		if (endDate != 0.0)
		{
			std::cout << sEndDate << " " << __LINE__ <<std::endl;
			endDate = double(Tools::String2Date(sEndDate.c_str()));
		}

	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << " " << __LINE__<< std::endl;
	}
	try
	{
		std::cout << "beginDate = " << beginDate << "  endDate = " << endDate << " LineNo = " << __LINE__ << std::endl;
		//时间转换：从local 转换为 UTC
		beginDate = Tools::ConvertLocal2UTC(this->pSession, beginDate);
		endDate = Tools::ConvertLocal2UTC(this->pSession, endDate);
		PRINTLINE("");
		// 4. Create IO2GRequestFactory:
		O2G2Ptr<IO2GRequestFactory> factory = this->pSession->getRequestFactory();
		if (!factory)
		{
			std::cout << "Cannot create request factory" << std::endl;
			return false;
		}
		PRINTLINE("")
		//5. Get IO2GTimeframeCollection:
		O2G2Ptr<IO2GTimeframeCollection> timeframeCollection = factory->getTimeFrameCollection();
		/*
		//观察有多少种TimeFrame
		for (int i = 0; i < timeframeCollection->size(); i++)
		{
			std::cout << "Timeframe = " << timeframeCollection->get(i)->getUnit() << " " << std::endl;
		}
		*/
		// 6. Get the IO2GTimeframe you need:
		O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get(stimeFrame.c_str());
		
		if (!timeFrame)
		{
			std::cout << "Timeframe '" << stimeFrame << "' is incorrect!" << std::endl;
			return false;
		}

		// 7. Create a market data snapshot request (IO2GRequest) 
		//using the instrument, time frame, and maximum number of bars as arguments:

		O2G2Ptr<IO2GRequest> request = factory->createMarketDataSnapshotRequestInstrument(
			instrument.c_str(),
			timeFrame,
			maxBars);
		if (!request)
		{
			std::cout << "Cannot create request : " 
				<< instrument.c_str() << " "
				<< timeFrame << " "
				<< maxBars << " "
				<<__LINE__<< std::endl;
			return false;
		}
		PRINTLINE("")
		// 记录该请求的相关信息到map中
		sFxcmRequestHisPrices reqData;
		reqData.instrument = instrument;
		reqData.stimeFrame = stimeFrame;
		reqData.beginDate = beginDate;
		reqData.endDate = endDate;
		reqData.getNums = 0;//已接受数据条数
		pResponseListener->mRequestDataSet[string(request->getRequestID())] = reqData;

		PRINTLINE("")
		// 8.Fill the market data snapshot request 
		//using the request, date and time "from", and date and time "to" as arguments

		//非阻塞模式的实现，与ResponseListener配合，该模式下只需要发送请求就返回

		//填充request
		PRINTLINE("")
		factory->fillMarketDataSnapshotRequestTime(request, beginDate, endDate, false);
		//记录request id，没有用处
		pResponseListener->setRequestID(request->getRequestID());
		PRINTLINE("")
		pSession->sendRequest(request);
		PRINTLINE("")
	}
	catch (const std::exception&e)
	{
		std::cout << e.what() << " " << __LINE__ << std::endl;
	}
	
}
// 查询最近的1笔历史数据
bool HxcmApi::qryLastHisPrice(string instrument, string stimeFrame)
{
	//开始时间设定为：当前时间 + 一天
	DATE beginDate = COleDateTime::GetCurrentTime();
	COleDateTimeSpan ds;
	ds.SetDateTimeSpan(1, 0, 0, 0);
	beginDate = beginDate + ds;//时间加上1天,确保大于当前时间
	//将本地时间转换为UTC时间,在qryHisPrices中统一转换
	//beginDate = Tools::ConvertLocal2UTC(this->pSession,beginDate);

	//std::cout << beginDate << " Line no = " << __LINE__ << std::endl;
	string sBeginDate;
	Tools::formatDate(beginDate, sBeginDate);
	//std::cout << "beginDate :" <<sBeginDate<< "   " << beginDate <<" Line no = "<< __LINE__ <<std::endl;
	return qryHisPrices(instrument, stimeFrame, 100, sBeginDate, "0");
}

// 查询账号信息
void HxcmApi::qryAccounts()
{
	O2G2Ptr<IO2GLoginRules> loginRules = this->pSession->getLoginRules();

	// Check if Accounts table is loaded automatically
	if (loginRules && loginRules->isTableLoadedByDefault(Accounts))
	{
		// If table is loaded, use getTableRefreshResponse method
		O2G2Ptr<IO2GResponse> accountsResponse = loginRules->getTableRefreshResponse(Accounts);
		O2G2Ptr<IO2GResponseReaderFactory> responseFactory = pSession->getResponseReaderFactory();
		if (responseFactory)
		{
			O2G2Ptr<IO2GAccountsTableResponseReader> accountsReader = responseFactory->createAccountsTableReader(accountsResponse);
			for (int i = 0; i < accountsReader->size(); i++)
			{
				O2G2Ptr<IO2GAccountRow> account = accountsReader->getRow(i);
				//std::cout << " AccountID = " << account->getAccountID() <<
				//	" Balance = " << account->getBalance() <<
				//	" UsedMargin = " << account->getHadgeMarginPCT() << std::endl;
			}
		}
	}
}

//定义货币对的offer信息
void HxcmApi::Subscribe(string instrument,string status,bool isInstrumentName=true)
{
	if (!this->pSessionStatusListener->isConnected())
	{
		//"与交易服务器的链接已经断开，无法订阅货币对的交易信息。",不能是中文
		//this->sendMessage("unconnected with fxcm server ,can not subscribe instrument offers info");
		return;
 	}
	//是否为货币对名称，即“USD/CNH”,如果是则要查询其OfferID
	if (isInstrumentName)
	{
		//true为打印货币对状态清单，用于调试用
		IO2GOfferRow * offer = getOfferAndPrint(pSession, instrument.c_str(),true );
		instrument = offer->getOfferID();
	}
	//1. 生成订阅请求
	//获取RequestFactory
	O2G2Ptr<IO2GRequestFactory> requestFactory = this->pSession->getRequestFactory();
	//
	if (!requestFactory)
	{
		std::cout << "Cannot create request factory" << std::endl;
		return;
	}
	O2G2Ptr<IO2GValueMap> valuemap = requestFactory->createValueMap();
	valuemap->setString(Command, O2G2::Commands::SetSubscriptionStatus);
	valuemap->setString(SubscriptionStatus, status.c_str());
	valuemap->setString(OfferID, instrument.c_str());
	O2G2Ptr<IO2GRequest> request = requestFactory->createOrderRequest(valuemap);
	if (!request)
	{
		std::cout << requestFactory->getLastError() << std::endl;
		return ;
	}	

	//2. 发送订阅请求
	pResponseListener->setRequestID(request->getRequestID());
	pSession->sendRequest(request);
}

//启动OnTick事件
void HxcmApi::StartTick(string timeFrame)
{
	PRINTLINE("Call HxcmApi::StartTick");
	//如果timeFrame对应的线程不存在
	if (this->TickThread.count(timeFrame) == 0)
	{
		function1<void, string> f = boost::bind(&HxcmApi::qryTickData, this, timeFrame);
		thread t( f,timeFrame );
		this->TickThread[timeFrame] = &t;
	}
	else
	{
		PRINTLINE("need not call StartTick again")
		string s1 = "need not call StartTick: ";// " again";
		s1 += timeFrame;
		s1 += " again";
		sendMessage(s1);
	}
}

//线程函数,功能定时查询
void HxcmApi::qryTickData()
{
	PRINTLINE("begin call qryTickData")
	while (1)
	{
		string timeFrame = "m1";
		std::cout << timeFrame << __LINE__ << std::endl;
		time_t ts = time(0);
		tm *ltm = localtime(&ts);
		std::cout << "时间: " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << std::endl;
		if (timeFrame == "m1")
		{
			int sleeptime = 1 * 10 * 1000; // 一分钟
			boost::timer::cpu_timer ts;
			std::list<string>::iterator iter = Tick_instruments[timeFrame].begin();
			while (iter != Tick_instruments[timeFrame].end())
			{
				std::cout << *iter << " " << timeFrame << "  " << __LINE__ << std::endl;
				//qryLastHisPrice(*iter, timeFrame);
				//qryHisPrices(*iter, timeFrame, 1, "2018-09-04 23:01:17", "2018-09-05 23:01:17");
				iter++;
			}
			PRINTLINE("")
				boost::timer::cpu_times tt = ts.elapsed();//时间单位均为纳秒
			float ff = sleeptime - tt.wall / 1000000;
			std::cout << ff << "  " << __LINE__ << std::endl;
			Sleep(ff);
		}
	}
	
}
void HxcmApi::qryTickData(string timeFrame)
{
	PRINTLINE("========Call HxcmApi::qryTickData============");
	int times = 60;
	if (timeFrame == "m1")
	{
		times = 60;
	}
	else if( timeFrame == "m5")
	{
		times = 5 * 60;
	}
	else if (timeFrame == "m10" )
	{
		times = 10 * 60;
	}
	else if (timeFrame == "m15")
	{
		times = 15 * 60;
	}
	//建立自己的session
	IO2GSession * pSession = CO2GTransport::createSession();
	SessionStatusListener *pSessionStatusListener = new SessionStatusListener(pSession,
																				this->UserName.c_str(),
																				this->PWD.c_str());
	pSession->subscribeSessionStatus(pSessionStatusListener);


	ResponseListener *pResponseListener = new ResponseListener(pSession, this);
	pSession->subscribeResponse(pResponseListener);

	pSession->login(UserName.c_str(), PWD.c_str(), URL.c_str(), CONN.c_str());
	pSessionStatusListener->waitEvents();

	if (pSessionStatusListener->isConnected())
	{
		PRINTLINE("Login successed");
	}

	while (1)
	{
		PRINTLINE("begin call qryTickData")
		time_t ts = time(0);
		tm *ltm = localtime(&ts);
		std::cout << "时间: " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << std::endl;

		int sleeptime = 1 * times * 1000; // 
		boost::timer::cpu_timer cts;
		std::list<string>::iterator iter = Tick_instruments[timeFrame].begin();
		while (iter != Tick_instruments[timeFrame].end())
		{
			std::cout << *iter << " " << timeFrame << "  " << __LINE__ << std::endl;
			//====================================================================================
			string sbeginDate = "2018-09-15 23:01:17";
			string sEndDate = "2018-09-16 23:01:17";
			string stimeFrame = "m1";
			string instrument = *iter;
			int maxBars = 1;
			// 处理fromDate，toDate为0的情况
			//如果beginDate和EndDate 等于0，即为当下
			//时间转换
			DATE beginDate = -1;
			DATE endDate = -1;
			try
			{
				beginDate = atof(sbeginDate.c_str());
				std::cout << beginDate << " : " << sbeginDate << " " << __LINE__ << std::endl;
				if (beginDate != 0.0)
				{
					beginDate = double(Tools::String2Date(sbeginDate.c_str()));
					std::cout << "line no =" << __LINE__ << std::endl;
				}

				endDate = atof(sEndDate.c_str());
				//std::cout << endDate << " " << __LINE__ << std::endl;
				if (endDate != 0.0)
				{
					std::cout << sEndDate << " " << __LINE__ << std::endl;
					endDate = double(Tools::String2Date(sEndDate.c_str()));
				}

			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << " " << __LINE__ << std::endl;
			}
			try
			{
				std::cout << "beginDate = " << beginDate << "  endDate = " << endDate << " LineNo = " << __LINE__ << std::endl;
				//时间转换：从local 转换为 UTC
				beginDate = Tools::ConvertLocal2UTC(pSession, beginDate);
				endDate = Tools::ConvertLocal2UTC(pSession, endDate);
				PRINTLINE("");
				// 4. Create IO2GRequestFactory:
				O2G2Ptr<IO2GRequestFactory> factory = pSession->getRequestFactory();
				if (!factory)
				{
					std::cout << "Cannot create request factory" << std::endl;
					return ;
				}
				
				//5. Get IO2GTimeframeCollection:
				PRINTLINE("5. Get IO2GTimeframeCollection:");
				O2G2Ptr<IO2GTimeframeCollection> timeframeCollection = factory->getTimeFrameCollection();

				// 6. Get the IO2GTimeframe you need:
				PRINTLINE("6. Get the IO2GTimeframe you need:");
				O2G2Ptr<IO2GTimeframe> timeFrame = timeframeCollection->get("m1");

				// 7. Create a market data snapshot request (IO2GRequest) 
				PRINTLINE("7. Create a market data snapshot request (IO2GRequest)");
				//using the instrument, time frame, and maximum number of bars as arguments:
				O2G2Ptr<IO2GRequest> request = NULL;
				try
				{
					std::cout << instrument << std::endl;
					request = factory->createMarketDataSnapshotRequestInstrument(instrument.c_str(),
																				timeFrame,
																				1);
				}
				catch (const std::exception& ee)
				{
					PRINTLINE(ee.what());
				}
				
				PRINTLINE(request->getRequestID());
				// 记录该请求的相关信息到map中
				sFxcmRequestHisPrices reqData;
				reqData.instrument = instrument;
				reqData.stimeFrame = stimeFrame;
				reqData.beginDate = beginDate;
				reqData.endDate = endDate;
				reqData.getNums = 0;//已接受数据条数
				pResponseListener->mRequestDataSet[string(request->getRequestID())] = reqData;

				PRINTLINE("")
				// 8.Fill the market data snapshot request 
				//using the request, date and time "from", and date and time "to" as arguments

				//非阻塞模式的实现，与ResponseListener配合，该模式下只需要发送请求就返回

				//填充request
				PRINTLINE("")
				factory->fillMarketDataSnapshotRequestTime(request, beginDate, endDate, false);

				PRINTLINE("")
				pSession->sendRequest(request);
				PRINTLINE("")
			}
			catch (const std::exception&e)
			{
				std::cout << e.what() << " " << __LINE__ << std::endl;
			}

			//======================================================================================

			iter++;
		}

		boost::timer::cpu_times tt = cts.elapsed();//时间单位均为纳秒
		float ff = sleeptime - tt.wall / 1000000;
		PRINTLINE(ff)
			Sleep(ff);
	}
	

}


///-------------------------------------------------------------------------------------
///Boost.Python封装，给python调用的函数
///-------------------------------------------------------------------------------------

struct HxcmApiWrap : HxcmApi, wrapper<HxcmApi>
{
	HxcmApiWrap() : HxcmApi() {};
	HxcmApiWrap(string  userName, string   pwd, string  url, string connection) 
		: HxcmApi(userName,pwd,url,connection)
	{
	};
	virtual void onResGetHistoryPrices(dict data, bool last)
	{
		//std::cout << "HxcmApiWrap::processGetHisPrices  " << __LINE__ << std::endl;
		try
		{
			this->get_override("onResGetHistoryPrices")(data, last);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onSubscribeInstrument(boost::python::list data)
	{
		//std::cout << "HxcmApiWrap::onSubscribeInstrument  " << __LINE__ << std::endl;
		try
		{
			this->get_override("onSubscribeInstrument")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onMessage(boost::python::dict data)
	{
		//std::cout << "HxcmApiWrap::onMessage  " << __LINE__ << std::endl;
		try
		{
			this->get_override("onMessage")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

};
/**/
// 注意python中继承类的构造函数也要相应变化，否则调试脚本会出错
BOOST_PYTHON_MODULE(hxcmapi)
{
	PyEval_InitThreads();	//导入时运行，保证先创建GIL
	class_<HxcmApiWrap, boost::noncopyable>("HxcmApi",init<>())
		.def(init<string,string,string, string>())
		.def("Login", &HxcmApiWrap::Login)
		.def("Logout", &HxcmApiWrap::Logout)
		.def("qryHisPrices",&HxcmApiWrap::qryHisPrices)
		.def("qryLastHisPrice",&HxcmApiWrap::qryLastHisPrice)
		.def("qryAccounts",&HxcmApiWrap::qryAccounts)
		.def("Subscribe",&HxcmApiWrap::Subscribe)
		.def("StartTick", &HxcmApiWrap::StartTick)
		.def("onResGetHistoryPrices", pure_virtual(&HxcmApiWrap::onResGetHistoryPrices))
		.def("onSubscribeInstrument",pure_virtual(&HxcmApiWrap::onSubscribeInstrument))
		.def("onMessage", pure_virtual(&HxcmApiWrap::onMessage))

		;
};


