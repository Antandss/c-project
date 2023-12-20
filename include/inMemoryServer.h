#ifndef INMEMORYSERVER_H
#define INMEMORYSERVER_H

#include "database.h"
#include "messageHandler.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

class inMemoryServer : public database {
    
    public:

        inMemoryServer();

        void listNewsgroups(MessageHandler& mh);

        bool createNewsgroup(string title);

        bool deleteNewsgroup(int id);

        bool listArticles(int id, MessageHandler& mh);

        bool createArticle(int id, string title, string author, string text);

        int deleteArticle(int groupId, int articleId);

        int getArticle(int groupId, int articleId, MessageHandler& mh);
    
    protected:
    
        std::map<Newsgroup, std::vector<Article>>::iterator ng_exist_by_id(int id){
            return find_if(db.begin(), db.end(), [&id](std::pair<Newsgroup, std::vector<Article>> p){
                    return p.first.id == id;
                });
        };

    
    private:
        std::map<Newsgroup, std::vector<Article>> db;
        int ngId;
        int aId;
};

#endif