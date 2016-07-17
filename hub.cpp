/*************************************************************************
	> File Name: hub.cpp
	> Author:  YinWen
	> Mail: yinwenatbit@163.com
	> Created Time: Fri 15 Jul 2016 08:56:05 PM CST
 ************************************************************************/


#include "codec.h"
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <boost/bind.hpp>
#include <unordered_map>
#include <set>

using namespace muduo;
using namespace muduo::net;

class Topic: public muduo::copyable
{
public:
    Topic(string topic)
    :topic_(topic)
    {}

    void add_conn(const TcpConnectionPtr &conn)
    {
        conns_.insert(conn);
    }

    void remove_conn(const TcpConnectionPtr &conn)
    {
        auto it = conns_.find(conn);
        if(it != conns_.end())
            conns_.erase(it);
    }
    
    void publish(string &content, Timestamp &this_time)
    {
        content_ = content;
        lastupdate_ = this_time;
        string message = get_message();
        for(auto it = conns_.begin(); it != conns_.end(); it++)
            (*it)->send(message);
    }

private:
    string get_message()
    {
        return "pub "+topic_+"\r\n"+content_+"\r\n";
    }
    string topic_;
    string content_;
    Timestamp lastupdate_;
    std::set<TcpConnectionPtr> conns_;
    

};



class HubServer: public boost::noncopyable
{
public:
    HubServer(EventLoop * loop, InetAddress & addr)
    : server_(loop, addr, "HubServer")
    {
        server_.setConnectionCallback(
            boost::bind(HubServer::onConnection, this, _1)
            );
        server_.setMessageCallback(
            boost::bind(HubServer::onMessage, this, _1, _2, _3)
            );
        boost::function<void()> fn = boost::bind(HubServer::time_Publisher, this);
        loop->runEvery(1.0, fn);
    }
    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr & conn)
    {
        if(conn->connected())
        {
            conn->setContext(std::set<string> ());
        }
        else
        {
            const std::set<string> &conn_sub = boost::any_cast<const std::set<string> >(conn->getContext());
            for(auto it = conn_sub.begin(); it != conn_sub.end(); it++)
            {
                doUnsubscribe(*it, conn);
            }
        }
    }

    void onMessage(const TcpConnectionPtr & conn,
                  Buffer *buff,
                  Timestamp receivetime)
    {
        ParseRes res = ParseRes::kSuccess;
        while(res == ParseRes::kSuccess)
        {
            string cmd;
            string topic;
            string content;
            res = parse_message(buff, cmd, topic, content);
            if(res == ParseRes::kSuccess)
            {
                if(cmd == "pub")
                {
                    get_topic(topic).publish(content, receivetime);
                }
                else if(cmd == "sub")
                {
			LOG_INFO << conn->name() << " subscribes " << topic;
                    doSubscribe(topic, conn);
                }
                else if(cmd == "unsub")
                {
                    doUnsubscribe(topic, conn);
                }
                else
                {
                    conn->shutdown();
                }
            }
            else if(res == ParseRes::kError)
            {
                conn->shutdown();
            }
        }
    }

    void time_Publisher()
    {
        Timestamp now = Timestamp::now();
        Topic &topic_time = get_topic("utc_time");
        string content = now.toFormattedString();
        topic_time.publish(content, now);
    }

    void doSubscribe(const string &topic, const TcpConnectionPtr & conn)
    {
        Topic &topic_need = get_topic(topic);
        topic_need.add_conn(conn);
            
        std::set<string> *conn_sub = boost::any_cast<std::set<string> >(conn->getMutableContext());
        conn_sub->insert(topic);
    }

    void doUnsubscribe(const string & topic, const TcpConnectionPtr &conn)
    {
        Topic &topic_need = get_topic(topic);
        topic_need.remove_conn(conn);
            
        std::set<string> *conn_sub = boost::any_cast<std::set<string> >(conn->getMutableContext());
        conn_sub->erase(topic);
    }

    Topic & get_topic(const string &topic)
    {
        auto it = topics_.find(topic);
        if(it == topics_.end())
        {
            it = topics_.insert(make_pair(topic, Topic(topic))).first;
        }
        return it->second;
    }

    TcpServer server_;
    std::unordered_map<string, Topic> topics_;

};

int main(int argc, char * argv[])
{
    if(argc <1)
    {
        printf("usage: %s port", argv[0]);
        exit(1);
    }

    uint16_t port = atoi(argv[1]);

    EventLoop loop;
    InetAddress addr(port);
    HubServer server(&loop, addr);
    server.start();
    loop.loop();
}
