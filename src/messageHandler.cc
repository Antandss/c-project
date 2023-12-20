#include <iostream>
#include "../include/protocol.h"
#include "../include/messageHandler.h"
#include <string>
#include "../include/connectionclosedexception.h"

/**

Message handler is the low-level message handler for messages to/from the
server. It handles transmission/reception of low-level message units.
*/
MessageHandler::MessageHandler(Connection *conn) {
    this->conn = conn;
}


void MessageHandler::sendByte(int code) {
        try {
        conn->write(static_cast<char>(code));
        } catch (std::exception const& e) {
        throw ConnectionClosedException();
        }
}

/**
Transmit a code (a constant from the Protocol class).
*/
void MessageHandler::sendCode(int code) {
        sendByte(code);
}

/**
Transmit an int value, according to the protocol.
*/
void MessageHandler::sendInt(int value) {
        sendByte((value >> 24) & 0xFF);

        sendByte((value >> 16) & 0xFF);

        sendByte((value >> 8) & 0xFF);

        sendByte(value & 0xFF);

}

/**
Transmit an int parameter, according to the protocol.
*/
void MessageHandler::sendIntParameter(int param) {
        sendCode(static_cast<int>(Protocol::PAR_NUM));
        sendInt(param);
}

/**
Transmit a string parameter, according to the protocol.
*/
void MessageHandler::sendStringParameter(std::string param) {
        sendCode(static_cast<int>(Protocol::PAR_STRING));
        sendInt(param.length());
        for (int i = 0; i < param.length(); i++) {
                sendByte(param[i]);
        }
}

int MessageHandler::recvByte() {
        int code = conn->read();
        if (!(conn->isConnected())) {
                throw ConnectionClosedException();
        }
        return code;
}

/**
Receive a command code or an error code from the server.
*/
int MessageHandler::recvCode() {
        int code = recvByte();
        return code;
}

/**
Receive an int value from the server.
*/
int MessageHandler::recvInt() {
        int b1 = recvByte();
        int b2 = recvByte();
        int b3 = recvByte();
        int b4 = recvByte();
        return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

/**
 * Receive an int parameter from the server.
*/
int MessageHandler::recvIntParameter() {
        int code = recvCode();
        if (code != static_cast<int>(Protocol::PAR_NUM)) {
                throw ConnectionClosedException();
        }
        return recvInt();
}

/**
 * Receive a string parameter from the server.
 */
std::string MessageHandler::recvStringParameter() {
        int code = recvCode();
        if (code != static_cast<int>(Protocol::PAR_STRING)) {
                throw ConnectionClosedException();
        }
        int n = recvInt();
        if (n < 0) {
                throw ConnectionClosedException();
        }
        std::string result(n, '\0');
        for (int i = 0; i < n; i++) {
                char ch = conn->read();
                result[i] = ch;
        }
        return result;
}