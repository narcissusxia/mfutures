g++ -o test TraderSpi.cpp MdSpi.cpp test2.cpp  ./lib_json/*.cpp -I./ -I /home/xialei/work/websocketpp/ -L/usr/lib/x86_64-linux-gnu/ -lboost_locale -lboost_system -lboost_random -lboost_thread  -lboost_chrono -lrt -pthread -L/home/xialei/work/TraderAPI/ -lthostmduserapi -lthosttraderapi -lPocoUtil -lPocoXML -lPocoNet -lPocoFoundation -I/include
export LD_LIBRARY_PATH=/home/xialei/work/TraderAPI:$LD_LIBRARY_PATH
./test ws://192.168.1.6:8080/futures/websocket/cppclient002
