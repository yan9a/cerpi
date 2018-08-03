g++ -std=c++11 -I /usr/local/mysql/connector-c++-8.0/include -L /usr/local/mysql/connector-c++-8.0/lib64 mysqltest.cpp -lmysqlcppconn8 -o mysqltest
./mysqltest
