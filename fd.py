import tushare as ts
#import PyMysqlPool #import ConnectionPool

#from PyMysqlPool.db_util.mysql_util import insertOrUpdate
from pymysqlpool import ConnectionPool

#git clone https://github.com/0xE8551CCB/pymysqlpool.git
# python3 setup.py install
db_config = {
    'local': {
        'host': "47.96.149.27", 'port': 3306,
        'user': "dbadmin", 'passwd': "System123456",
        'db': "future", 'charset': "utf8",
        'pool': {
            #use = 0 no pool else use pool
            "use":1,
            # size is >=0,  0 is dynamic pool
            "size":0,
            #pool name
            "name":"local",
        }
    },
    'poi': {
        'host': "10.95.130.***", 'port': 8787,
        'user': "lujunxu", 'passwd': "****",
        'db': "poi_relation", 'charset': "utf8",
        'pool': {
            #use = 0 no pool else use pool
            "use":0,
            # size is >=0,  0 is dynamic pool
            "size":0,
            #pool name
            "name":"poi",
        }
    },
}

config = {
    'pool_name': 'test',
    'host': '47.96.149.27',
    'port': 3306,
    'user': 'dbadmin',
    'password': 'System123456',
    'database': 'future',
    'pool_resize_boundary': 50,
    'enable_auto_resize': True
}

def connection_pool():
    # Return a connection pool instance
    pool = ConnectionPool(**config)
    return pool

#engine = create_engine('mysql+pymysql://dbadmin:System123456@47.96.149.27/future?charset=utf8')
#

df = ts.get_future_daily('20180101','20180204','CFFEX')
print(df)
for index, row in df.iterrows():   # 获取每行的index、row
    if row['open'] == '' :
        row['open'] = row['close']
        row['high'] = row['close']
        row['low'] =row['close']
    if row['symbol'] == 'AUefp' :  
        continue

    print(row['symbol'])
    print(row['turnover'])
    with connection_pool().cursor() as cursor:
     	    result = cursor.execute('INSERT INTO  k_dateline_data\
     		(symbol,TradingDate,Open,High,Low\
     		,Close,Volume,OpenInterest,Turnover,Settle\
     		,PreSettle,Variety) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)',\
            (row['symbol'],row['date'],row['open'],row['high'],row['low']\
            ,row['close'],row['volume'],row['open_interest'],row['turnover'],row['settle']\
            ,row['pre_settle'],row['variety']))
        	 

    
    






    #_args = (row['open'], row['high'])
    #_sql = """INSERT INTO user (Open, High) VALUES (%lf, %lf)"""
    #affect = insertOrUpdate(db_config['local'], _sql, _args)
        #print(result, cursor.lastrowid)
     #print(index,row)
#df.to_sql('k_dateline_data',engine,if_exists='append')
#df = ts.get_shfe_vwap('20180131')


