#ifndef MESSAGEHANDLER_H
#include "connection.h"
#define MESSAGEHANDLER_H


#include <string> 


class MessageHandler{

    public:
        MessageHandler(Connection *conn);
        void sendByte(int code);
        void sendCode(int code);
        void sendInt(int value);
        void sendIntParameter(int param);
        void sendStringParameter(std::string param);
        int recvByte();
        int recvCode();
        int recvInt();
        int recvIntParameter();
        std::string recvStringParameter();

    private:
        Connection *conn;

};

#endif