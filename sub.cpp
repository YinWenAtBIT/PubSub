/*************************************************************************
	> File Name: sub.cpp
	> Author:  YinWen
	> Mail: yinwenatbit@163.com
	> Created Time: Sun 17 Jul 2016 04:26:16 PM CST
 ************************************************************************/


#include "pubsub.h"


std::vector<string> topics;

void Message(string &topic, string & content, Timestamp & receivetime)
{
    printf("%s : %s\n", topic.c_str(), content.c_str());
}


void Connection(PubsubClient * client)
{
    for(auto it = topics.begin();
       it!= topics.end();
       it++)
    {
        client->subscribe(*it, Message);
    }
}



int main(int argc, char * argv[])
{
    if(argc<3)
    {
        printf("usage: %s hostip:port topic [topic]", argv[0]);
        exit(1);
    }

    string hostport = argv[1];
    size_t seprate = hostport.find(':');

    if(seprate == string::npos)
    {
        printf("error input");
        exit(1);
    }

    string host = hostport.substr(0, seprate);
    uint16_t port = atoi(hostport.c_str()+seprate+1);
    InetAddress addr(host, port); 
    
    for(int i=2; i<argc; i++)
    {
        topics.push_back(argv[i]);
    }

    EventLoop loop;
    string name = "sub_client";
    PubsubClient client(&loop, addr, name);
    client.setConnectionCallback(Connection);
    client.start();
    loop.loop();
}

