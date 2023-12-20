#include "../include/database.h"
#include "../include/messageHandler.h"
#include "../include/connectionclosedexception.h"
#include "../include/protocol.h"
#include "../include/inMemoryServer.h"
#include "../include/server.h"
#include "../include/diskServer.h"
#include <string>
#include <algorithm>


#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;


Server init(int argc, char* argv[])
{
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]);
        } catch (std::exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

void listNewsgroups(MessageHandler& mh, database* db) {
    mh.sendCode(static_cast<int>(Protocol::ANS_LIST_NG));
    
    db->listNewsgroups(mh);

    mh.sendCode(static_cast<int>(Protocol::ANS_END));
}

void createNewsgroup(MessageHandler& mh, database* db){
    mh.sendCode(static_cast<int>(Protocol::ANS_CREATE_NG));

    string title = mh.recvStringParameter();
    if(db->createNewsgroup(title)){
        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
    } else {
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
        mh.sendCode(static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS));
    }

    mh.sendCode(static_cast<int>(Protocol::ANS_END));

}

void deleteNewsgroup(MessageHandler& mh, database* db) {
    mh.sendCode(static_cast<int>(Protocol::ANS_DELETE_NG));
    int idNbr = mh.recvIntParameter();
    if(db->deleteNewsgroup(idNbr)){
        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
    } else{
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
        mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
    }
    mh.sendCode(static_cast<int>(Protocol::ANS_END));
}



void listArticles(MessageHandler& mh, database* db) {
    mh.sendCode(static_cast<int>(Protocol::ANS_LIST_ART));

    int aId = mh.recvIntParameter();

    if(db->listArticles(aId, mh)){
        //mh.sendInt(static_cast<int>(Protocol::ANS_ACK));
    } else {
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
        mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
    }

    mh.sendCode(static_cast<int>(Protocol::ANS_END));

}

void createArticle(MessageHandler& mh, database* db){
    mh.sendCode(static_cast<int>(Protocol::ANS_CREATE_ART));

    int idNbr = mh.recvIntParameter();
    string title = mh.recvStringParameter();
    string author = mh.recvStringParameter();
    string text = mh.recvStringParameter();

    if(db->createArticle(idNbr, title, author, text)){
        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
    } else {
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
        mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
    }

    mh.sendCode(static_cast<int>(Protocol::ANS_END));


}

void deleteArticle(MessageHandler& mh, database* db){
    mh.sendCode(static_cast<int>(Protocol::ANS_DELETE_ART));
    int groupId = mh.recvIntParameter();
    int articleId = mh.recvIntParameter();
    int n = db->deleteArticle(groupId, articleId);

    if( n == 1){
        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
    } else {
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
            if( n == 2){
                mh.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
            } 
            if (n == 3){
                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
            }
    }
    mh.sendCode(static_cast<int>(Protocol::ANS_END));
}

void getArticle(MessageHandler& mh, database* db) {
    mh.sendCode(static_cast<int>(Protocol::ANS_GET_ART));
    int groupId = mh.recvIntParameter();
    int articleId = mh.recvIntParameter();
    int n = db->getArticle(groupId, articleId, mh);

    if( n != 1){
        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
            if( n == 2){
                mh.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
            } 
            if (n == 3){
                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
            }
    }
    mh.sendCode(static_cast<int>(Protocol::ANS_END));
}


int main(int argc, char* argv[])
{   
    auto server = init(argc, argv);
    
    database* db;

    cout << "Choose a database type:" << endl;
    cout << "1: Disk database" << endl;
    cout << "2: Memory database" << endl;
    int dbChoice;
    cin >> dbChoice;

    if(dbChoice == 1){
        db = new inMemoryServer();
        cout << "Memory version" << endl;
    } else {
        db = new diskServer();
        cout << "Disk version" << endl;
    }



    while (true) {
        auto conn = server.waitForActivity();

        if (conn != nullptr) {
            MessageHandler mh(conn.get());

            try {
                int n = mh.recvCode();

                switch(n) {
                    case 1:
                        listNewsgroups(mh, db);
                        break;
                    case 2:
                        createNewsgroup(mh, db);
                        break;
                    case 3:
                        deleteNewsgroup(mh, db);
                        break;
                    case 4:
                        listArticles(mh, db);
                        break;
                    case 5:
                        createArticle(mh, db);
                        break;
                    case 6:
                        deleteArticle(mh, db);
                        break;
                    case 7:
                        getArticle(mh, db);
                        break;
                    default:
                        cout << "Wrong command" << endl;
                        throw ConnectionClosedException();
                }
                if (mh.recvCode() != 8) {
                    cout << "Connection closed due to lack of affirmative answer." << endl;
                    throw ConnectionClosedException();
                }

            }
            catch(ConnectionClosedException&) {
                cout << "Unexpected command, connection terminated." << endl;
                server.deregisterConnection(conn);

            }
        } else {
            conn = make_shared<Connection>();
            server.registerConnection(conn);
            cout << "New clients connects" << endl;
        }
    }
    
}
