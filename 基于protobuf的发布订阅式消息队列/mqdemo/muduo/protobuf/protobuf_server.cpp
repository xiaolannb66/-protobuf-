#include "muduo/proto/codec.h"
#include "muduo/proto/dispatcher.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include "request.pb.h"
#include <iostream>
#include <unordered_map>

class Server {
    public:
        typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
        typedef std::shared_ptr<bit::TranslateRequest> TranslateRequestPtr;
        typedef std::shared_ptr<bit::AddRequest> AddRequestPtr;
        Server(int port): _server(&_baseloop, muduo::net::InetAddress("0.0.0.0", port), 
            "Server", muduo::net::TcpServer::kReusePort),
            _dispatcher(std::bind(&Server::onUnknownMessage, this, std::placeholders::_1, 
                std::placeholders::_2, std::placeholders::_3)),
            _codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &_dispatcher, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)){
            
            //注册业务请求处理函数
            _dispatcher.registerMessageCallback<bit::TranslateRequest>(std::bind(&Server::onTranslate, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                
            _dispatcher.registerMessageCallback<bit::AddRequest>(std::bind(&Server::onAdd, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

            _server.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &_codec,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            _server.setConnectionCallback(std::bind(&Server::onConnection, this, std::placeholders::_1));
        }
        void start() {
            _server.start();
            _baseloop.loop();
        }
    private:
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
        void onTranslate(const muduo::net::TcpConnectionPtr& conn, const TranslateRequestPtr& message, muduo::Timestamp) {
            //1. 提取message中的有效消息，也就是需要翻译的内容
            std::string req_msg = message->msg();
            //2. 进行翻译，得到结果
            std::string rsp_msg = translate(req_msg);
            //3. 组织protobuf的响应
            bit::TranslateResponse resp;
            resp.set_msg(rsp_msg);
            //4. 发送响应
            _codec.send(conn, resp);
        }
        void onAdd(const muduo::net::TcpConnectionPtr& conn, const AddRequestPtr& message, muduo::Timestamp) {
            int num1 = message->num1();
            int num2 = message->num2();
            int result = num1 + num2;
            bit::AddResponse resp;
            resp.set_result(result);
            _codec.send(conn, resp);
        }
        void onUnknownMessage(const muduo::net::TcpConnectionPtr& conn, const MessagePtr& message, muduo::Timestamp) {
            LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
            conn->shutdown();
        }
        void onConnection(const muduo::net::TcpConnectionPtr &conn) {
            if (conn->connected()) {
                LOG_INFO << "新连接建立成功！";
            }else {
                LOG_INFO << "连接即将关闭！";
            }
        }
    private:
        muduo::net::EventLoop _baseloop;
        muduo::net::TcpServer _server;//服务器对象
        ProtobufDispatcher _dispatcher;//请求分发器对象--要向其中注册请求处理函数
        ProtobufCodec _codec;//protobuf协议处理器--针对收到的请求数据进行protobuf协议处理
};

int main()
{
    Server server(8085);
    server.start();
    return 0;
}