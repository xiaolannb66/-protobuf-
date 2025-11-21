#include "include/muduo/net/TcpServer.h"
#include "include/muduo/net/EventLoop.h"
#include "include/muduo/net/TcpConnection.h" 
#include <iostream>
#include <functional>
#include <unordered_map>

class TranslateServer {
    public:
        TranslateServer(int port):_server(&_baseloop, 
            muduo::net::InetAddress("0.0.0.0", port), 
            "TranslateServer", muduo::net::TcpServer::kReusePort){

            //将我们的类成员函数，设置为服务器的回调处理函数
            // std::bind 是一个函数适配器函数，对指定的函数进行参数绑定
            _server.setConnectionCallback(std::bind(&TranslateServer::onConnection, this, std::placeholders::_1));
            _server.setMessageCallback(std::bind(&TranslateServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        }
        //启动服务器
        void start() {
            _server.start();//开始事件监听
            _baseloop.loop();//开始事件监控，这是一个死循环阻塞接口
        }
    private:
        //onConnection,应该是在一个连接，建立成功，以及关闭的时候被调用
        void onConnection(const muduo::net::TcpConnectionPtr&conn){
            //新连接建立成功时的回调函数
            if (conn->connected() == true) {
                std::cout << "新连接建立成功！\n";
            }else {
                std::cout << "新连接关闭！\n";
            }
        }
        std::string translate(const std::string &str) {
            static std::unordered_map<std::string, std::string> dict_map = {
                {"hello", "你好"},
                {"Hello", "你好"},
                {"你好", "Hello"},
                {"吃了吗", "油泼面"}
            };
            auto it = dict_map.find(str);
            if (it == dict_map.end()) {
                return "没听懂！！";
            }
            return it->second;
        }
        void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp) {
            //通信连接收到请求时的回调函数
            //1. 从buf中把请求的数据取出来
            std::string str = buf->retrieveAllAsString();
            //2. 调用translate接口进行翻译
            std::string resp = translate(str);
            //3. 对客户端进行响应结果
            conn->send(resp);
        }
    private:
        //_baseloop是epoll的事件监控，会进行描述符的事件监控，触发事件后进行io操作
        muduo::net::EventLoop _baseloop;
        //这个server对象，主要用于设置回调函数，用于告诉服务器收到什么请求该如何处理
        muduo::net::TcpServer _server;
};

int main()
{
    TranslateServer server(8085);
    server.start();
    return 0;
}