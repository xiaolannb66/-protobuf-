#include "include/muduo/net/TcpClient.h"
#include "include/muduo/net/EventLoopThread.h"
#include "include/muduo/net/TcpConnection.h" 
#include "include/muduo/base/CountDownLatch.h"
#include <iostream>
#include <functional>


class TranslateClient {
    public:
        TranslateClient(const std::string &sip, int sport):_latch(1),
            _client(_loopthread.startLoop(), muduo::net::InetAddress(sip, sport), "TranslateClient"){
            
            _client.setConnectionCallback(std::bind(&TranslateClient::onConnection, this, std::placeholders::_1));
            _client.setMessageCallback(std::bind(&TranslateClient::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        }
        //连接服务器---需要阻塞等待连接建立成功之后再返回
        void connect() {
            _client.connect();
            _latch.wait();//阻塞等待，直到连接建立成功
        }
        bool send(const std::string &msg) {
            if (_conn->connected()) {//连接状态正常，再发送，否则就返回false
                _conn->send(msg);
                return true;
            }
            return false;
        }   
    private:
        //连接建立成功时候的回调函数，连接建立成功后，唤醒上边的阻塞
        void onConnection(const muduo::net::TcpConnectionPtr&conn){
            if (conn->connected()) {
                _latch.countDown();//唤醒主线程中的阻塞
                _conn = conn;
            }else {
                //连接关闭时的操作
                _conn.reset();
            }
        }
        //收到消息时候的回调函数
        void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp) {
            std::cout << "翻译结果：" << buf->retrieveAllAsString() << std::endl;
        }
    private:
        muduo::CountDownLatch _latch;
        muduo::net::EventLoopThread _loopthread;
        muduo::net::TcpClient _client;
        muduo::net::TcpConnectionPtr _conn;
};

int main()
{
    TranslateClient client("127.0.0.1", 8085);
    client.connect();

    while(1) {
        std::string buf;
        std::cin >> buf;
        client.send(buf);
    }
    return 0;
}