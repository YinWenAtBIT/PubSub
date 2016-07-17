/*************************************************************************
	> File Name: codec.h
	> Author:  YinWen
	> Mail: yinwenatbit@163.com
	> Created Time: Fri 15 Jul 2016 09:10:06 PM CST
 ************************************************************************/

#ifndef MUDUO_CODE_H
#define MUDUO_CODE_H

#include <muduo/net/Buffer.h>
#include <muduo/base/Types.h>
using muduo::string;

enum  class ParseRes
{
    kSuccess = 0,
    kError,
    kCont
};

ParseRes parse_message(muduo::net::Buffer * buff,
                    string &cmd,
                    string &topic,
                    string &content
                    );


#endif
