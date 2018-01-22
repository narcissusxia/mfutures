g++ -o test echo_client.cpp  ./lib_json/*.cpp -I./ -I /home/xialei/work/websocketpp/ -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_random -lboost_thread  -lboost_chrono -lrt -pthread -L/home/xialei/work/TraderAPI/ -lthosttraderapi -lPocoUtil -lPocoXML -lPocoNet -lPocoFoundation -I/include
export LD_LIBRARY_PATH=/home/xialei/work/TraderAPI:$LD_LIBRARY_PATH
./test
