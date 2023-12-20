#ifndef DISKSERVER_H
#define DISKSERVER_H

#include "database.h"
#include "messageHandler.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

class diskServer : public database {
    
    public:

        diskServer();

        void listNewsgroups(MessageHandler& mh);

        bool createNewsgroup(string title);

        bool deleteNewsgroup(int id);

        bool listArticles(int id, MessageHandler& mh);

        bool createArticle(int id, string title, string author, string text);

        int deleteArticle(int groupId, int articleId);

        int getArticle(int groupId, int articleId, MessageHandler& mh);
        

    
    private:
        int ngId;
        int aId;
        const char* dir = "./database";
        void eraseFileLine(string path, string eraseLine);
};

#endif