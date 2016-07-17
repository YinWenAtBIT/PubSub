/*************************************************************************
	> File Name: pub.cpp
	> Author:  YinWen
	> Mail: yinwenatbit@163.com
	> Created Time: Sun 17 Jul 2016 03:12:41 PM CST
 ************************************************************************/

#include "pubsub.h"

string my_topic;
string my_content;
EventLoop * my_loop;

void Connection(PubsubClient * client)
{
    if(client->connected())
    {
        client->publish(my_topic, my_content);
        client->stop();
    }
    else
        my_loop->quit();
}

int main(int argc, char * argv[])
{
    if(argc<4)
    {
        printf("usage: %s hubip:port topic content", argv[0]);
        exit(1);
    }
    string hostport = argv[1];
    int seprate = hostport.find(':');
    if(seprate == string::npos)
    {
        printf("error input");
        exit(1);
    }
    string host = hostport.substr(0, seprate);
    uint16_t port = atoi(hostport.c_str()+seprate+1);
    InetAddress addr(host, port); 
    
    my_topic = argv[2];
    my_content = argv[3];

    EventLoop loop;
    my_loop = &loop;
    string name = "pub_client";
    PubsubClient client(my_loop, addr, name);
    client.setConnectionCallback(Connection);
    client.start();
    loop.loop();
}
