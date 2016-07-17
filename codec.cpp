/*************************************************************************
	> File Name: code.cpp
	> Author:  YinWen
	> Mail: yinwenatbit@163.com
	> Created Time: Fri 15 Jul 2016 09:36:51 PM CST
 ************************************************************************/

#include "codec.h"


ParseRes parse_message(muduo::net::Buffer * buff,
                    string &cmd,
                    string &topic,
                    string &content
                    )
{
    ParseRes res = ParseRes::kError;
    const char * crlf = buff->findCRLF();
    if(crlf)
    {
        const char * space = std::find(buff->peek(), crlf, ' ');
        if(space != crlf)
        {
            cmd.assign(buff->peek(), space);
            topic.assign(space +1, crlf);
            if(cmd == "pub")
            {
                const char *start = crlf+2;
                crlf = buff->findCRLF(start);
                if(crlf)
                {
                    content.assign(start, crlf);
                    buff->retrieveUntil(crlf+2);
                    res = ParseRes::kSuccess;
                }
                else
                {
                    res = ParseRes::kCont;
                }
            }
            else
            {
                buff->retrieveUntil(crlf+2);
                res = ParseRes::kSuccess;
            }
        }
        else
        {
            res = ParseRes::kError;
        }
    }
    else
    {
        res = ParseRes::kCont;
    }
    return res;
}
