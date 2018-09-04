from SmartTraderFrameWork.trader.language import constant as cst
import time
from logging import INFO


########################################################################
class BaseData(object):
    """回调函数推送数据的基础类，其他数据类继承于此"""
    def __init__(self):
        self.gatewayName = cst.EMPTY_STRING
        self.rawData = None


########################################################################
class TickData(BaseData):
    """Tick行情数据类"""
    def __init__(self):
        # 调用父类的构造函数
        super(TickData, self).__init__()

        # 代码相关
        self.symbol = cst.EMPTY_STRING          # 合约代码
        self.stSymbol = cst.EMPTY_STRING        # 合约在st系统中的唯一代码，通常为： 合约代码.交易所代码
        self.exchange = cst.EMPTY_STRING

        # 成交数据
        self.lastPrice = cst.EMPTY_FLOAT            # 最新成交价
        self.lastVolume = cst.EMPTY_INT             # 最新成交量
        self.volume = cst.EMPTY_INT                 # 今天总成交量
        self.openInterest = cst.EMPTY_INT           # 持仓量
        self.time = cst.EMPTY_STRING                # 时间 11:20:56.5
        self.date = cst.EMPTY_STRING                # 日期 20151009
        self.datetime = None                    # python的datetime时间对象

        # 常规行情
        self.openPrice = cst.EMPTY_FLOAT  # 今日开盘价
        self.highPrice = cst.EMPTY_FLOAT  # 今日最高价
        self.lowPrice = cst.EMPTY_FLOAT  # 今日最低价
        self.preClosePrice = cst.EMPTY_FLOAT

        self.upperLimit = cst.EMPTY_FLOAT  # 涨停价
        self.lowerLimit = cst.EMPTY_FLOAT  # 跌停价

        # 五档行情
        self.bidPrice1 = cst.EMPTY_FLOAT
        self.bidPrice2 = cst.EMPTY_FLOAT
        self.bidPrice3 = cst.EMPTY_FLOAT
        self.bidPrice4 = cst.EMPTY_FLOAT
        self.bidPrice5 = cst.EMPTY_FLOAT

        self.askPrice1 = cst.EMPTY_FLOAT
        self.askPrice2 = cst.EMPTY_FLOAT
        self.askPrice3 = cst.EMPTY_FLOAT
        self.askPrice4 = cst.EMPTY_FLOAT
        self.askPrice5 = cst.EMPTY_FLOAT

        self.bidVolume1 = cst.EMPTY_INT
        self.bidVolume2 = cst.EMPTY_INT
        self.bidVolume3 = cst.EMPTY_INT
        self.bidVolume4 = cst.EMPTY_INT
        self.bidVolume5 = cst.EMPTY_INT

        self.askVolume1 = cst.EMPTY_INT
        self.askVolume2 = cst.EMPTY_INT
        self.askVolume3 = cst.EMPTY_INT
        self.askVolume4 = cst.EMPTY_INT
        self.askVolume5 = cst.EMPTY_INT         


########################################################################
class BarData(BaseData):
    """K线数据"""
    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(BarData, self).__init__()

        self.stSymbol = cst.EMPTY_STRING  # st系统代码
        self.symbol = cst.EMPTY_STRING  # 代码
        self.exchange = cst.EMPTY_STRING  # 交易所

        self.open = cst.EMPTY_FLOAT  # OHLC
        self.high = cst.EMPTY_FLOAT
        self.low = cst.EMPTY_FLOAT
        self.close = cst.EMPTY_FLOAT

        self.date = cst.EMPTY_STRING  # bar开始的时间，日期
        self.time = cst.EMPTY_STRING  # 时间
        self.datetime = None  # python的datetime时间对象

        self.volume = cst.EMPTY_INT  # 成交量
        self.openInterest = cst.EMPTY_INT  # 持仓量    


########################################################################
class TradeData(BaseData):
    """成交数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(TradeData, self).__init__()

        # 代码编号相关
        self.symbol = cst.EMPTY_STRING  # 合约代码
        self.exchange = cst.EMPTY_STRING  # 交易所代码
        self.stSymbol = cst.EMPTY_STRING  # 合约在st系统中的唯一代码，通常是 合约代码.交易所代码

        self.tradeID = cst.EMPTY_STRING  # 成交编号
        self.stTradeID = cst.EMPTY_STRING  # 成交在st系统中的唯一编号，通常是 Gateway名.成交编号

        self.orderID = cst.EMPTY_STRING  # 订单编号
        self.stOrderID = cst.EMPTY_STRING  # 订单在st系统中的唯一编号，通常是 Gateway名.订单编号

        # 成交相关
        self.direction = cst.EMPTY_UNICODE  # 成交方向
        self.offset = cst.EMPTY_UNICODE  # 成交开平仓
        self.price = cst.EMPTY_FLOAT  # 成交价格
        self.volume = cst.EMPTY_INT  # 成交数量
        self.tradeTime = cst.EMPTY_STRING  # 成交时间


########################################################################
class OrderData(BaseData):
    """订单数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(OrderData, self).__init__()

        # 代码编号相关
        self.symbol = cst.EMPTY_STRING  # 合约代码
        self.exchange = cst.EMPTY_STRING  # 交易所代码
        self.stSymbol = cst.EMPTY_STRING  # 合约在st系统中的唯一代码，通常是 合约代码.交易所代码

        self.orderID = cst.EMPTY_STRING  # 订单编号
        self.stOrderID = cst.EMPTY_STRING  # 订单在st系统中的唯一编号，通常是 Gateway名.订单编号

        # 报单相关
        self.direction = cst.EMPTY_UNICODE  # 报单方向
        self.offset = cst.EMPTY_UNICODE  # 报单开平仓
        self.price = cst.EMPTY_FLOAT  # 报单价格
        self.totalVolume = cst.EMPTY_INT  # 报单总数量
        self.tradedVolume = cst.EMPTY_INT  # 报单成交数量
        self.status = cst.EMPTY_UNICODE  # 报单状态

        self.orderTime = cst.EMPTY_STRING  # 发单时间
        self.cancelTime = cst.EMPTY_STRING  # 撤单时间

        # CTP/LTS相关
        self.frontID = cst.EMPTY_INT  # 前置机编号
        self.sessionID = cst.EMPTY_INT  # 连接编号


########################################################################
class PositionData(BaseData):
    """持仓数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(PositionData, self).__init__()

        # 代码编号相关
        self.symbol = cst.EMPTY_STRING  # 合约代码
        self.exchange = cst.EMPTY_STRING  # 交易所代码
        self.stSymbol = cst.EMPTY_STRING  # 合约在st系统中的唯一代码，合约代码.交易所代码

        # 持仓相关
        self.direction = cst.EMPTY_STRING  # 持仓方向
        self.position = cst.EMPTY_INT  # 持仓量
        self.frozen = cst.EMPTY_INT  # 冻结数量
        self.price = cst.EMPTY_FLOAT  # 持仓均价
        self.stPositionName = cst.EMPTY_STRING  # 持仓在st系统中的唯一代码，通常是stSymbol.方向
        self.ydPosition = cst.EMPTY_INT  # 昨持仓
        self.positionProfit = cst.EMPTY_FLOAT  # 持仓盈亏


########################################################################
class AccountData(BaseData):
    """账户数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(AccountData, self).__init__()

        # 账号代码相关
        self.accountID = cst.EMPTY_STRING  # 账户代码
        self.stAccountID = cst.EMPTY_STRING  # 账户在st中的唯一代码，通常是 Gateway名.账户代码

        # 数值相关
        self.preBalance = cst.EMPTY_FLOAT  # 昨日账户结算净值
        self.balance = cst.EMPTY_FLOAT  # 账户净值
        self.available = cst.EMPTY_FLOAT  # 可用资金
        self.commission = cst.EMPTY_FLOAT  # 今日手续费
        self.margin = cst.EMPTY_FLOAT  # 保证金占用
        self.closeProfit = cst.EMPTY_FLOAT  # 平仓盈亏
        self.positionProfit = cst.EMPTY_FLOAT  # 持仓盈亏


########################################################################
class ErrorData(BaseData):
    """错误数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(ErrorData, self).__init__()

        self.errorID = cst.EMPTY_STRING  # 错误代码
        self.errorMsg = cst.EMPTY_UNICODE  # 错误信息
        self.additionalInfo = cst.EMPTY_UNICODE  # 补充信息

        self.errorTime = time.strftime('%X', time.localtime())  # 错误生成时间


########################################################################
class LogData(BaseData):
    """日志数据类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(LogData, self).__init__()

        self.logTime = time.strftime('%X', time.localtime())  # 日志生成时间
        self.logContent = cst.EMPTY_UNICODE  # 日志信息
        self.logLevel = INFO  # 日志级别


########################################################################
class ContractData(BaseData):
    """合约详细信息类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(ContractData, self).__init__()

        self.symbol = cst.EMPTY_STRING  # 代码
        self.exchange = cst.EMPTY_STRING  # 交易所代码
        self.stSymbol = cst.EMPTY_STRING  # 合约在st系统中的唯一代码，通常是 合约代码.交易所代码
        self.name = cst.EMPTY_UNICODE  # 合约中文名

        self.productClass = cst.EMPTY_UNICODE  # 合约类型
        self.size = cst.EMPTY_INT  # 合约大小
        self.priceTick = cst.EMPTY_FLOAT  # 合约最小价格TICK

        # 期权相关
        self.strikePrice = cst.EMPTY_FLOAT  # 期权行权价
        self.underlyingSymbol = cst.EMPTY_STRING  # 标的物合约代码
        self.optionType = cst.EMPTY_UNICODE  # 期权类型
        self.expiryDate = cst.EMPTY_STRING  # 到期日


########################################################################
class SubscribeReq(object):
    """订阅行情时传入的对象类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        self.symbol = cst.EMPTY_STRING  # 代码
        self.exchange = cst.EMPTY_STRING  # 交易所

        # 以下为IB相关
        self.productClass = cst.EMPTY_UNICODE  # 合约类型
        self.currency = cst.EMPTY_STRING  # 合约货币
        self.expiry = cst.EMPTY_STRING  # 到期日
        self.strikePrice = cst.EMPTY_FLOAT  # 行权价
        self.optionType = cst.EMPTY_UNICODE  # 期权类型


########################################################################
class OrderReq(object):
    """发单时传入的对象类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        self.symbol = cst.EMPTY_STRING  # 代码
        self.exchange = cst.EMPTY_STRING  # 交易所
        self.stSymbol = cst.EMPTY_STRING  # ST合约代码
        self.price = cst.EMPTY_FLOAT  # 价格
        self.volume = cst.EMPTY_INT  # 数量

        self.priceType = cst.EMPTY_STRING  # 价格类型
        self.direction = cst.EMPTY_STRING  # 买卖
        self.offset = cst.EMPTY_STRING  # 开平

        # 以下为IB相关
        self.productClass = cst.EMPTY_UNICODE  # 合约类型
        self.currency = cst.EMPTY_STRING  # 合约货币
        self.expiry = cst.EMPTY_STRING  # 到期日
        self.strikePrice = cst.EMPTY_FLOAT  # 行权价
        self.optionType = cst.EMPTY_UNICODE  # 期权类型     
        self.lastTradeDateOrContractMonth = cst.EMPTY_STRING  # 合约月,IB专用
        self.multiplier = cst.EMPTY_STRING  # 乘数,IB专用


########################################################################
class CancelOrderReq(object):
    """撤单时传入的对象类"""

    # ----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        self.symbol = cst.EMPTY_STRING  # 代码
        self.exchange = cst.EMPTY_STRING  # 交易所
        self.stSymbol = cst.EMPTY_STRING  # ST合约代码

        # 以下字段主要和CTP、LTS类接口相关
        self.orderID = cst.EMPTY_STRING  # 报单号
        self.frontID = cst.EMPTY_STRING  # 前置机号
        self.sessionID = cst.EMPTY_STRING  # 会话号