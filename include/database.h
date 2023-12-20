#ifndef DATABASE_H
#define DATABASE_H

#include "messageHandler.h"

#include <map>
#include <vector>
#include <string>

using namespace std; 

class database{
   
    public:
        
        virtual ~database() = default;

        virtual void listNewsgroups(MessageHandler& mh) = 0;

        virtual bool createNewsgroup(string title) = 0;

        virtual bool deleteNewsgroup(int id) = 0;

        virtual bool listArticles(int id, MessageHandler& mh) = 0;

        virtual bool createArticle(int id, string title, string author, string text) = 0;

        virtual int deleteArticle(int groupId, int articleId) = 0;

        virtual int getArticle(int groupId, int articleId, MessageHandler& mh) = 0;
    
    protected:

        struct Newsgroup{
            int id;
            std::string name;
            bool operator<(Newsgroup other) const {
                return id < other.id;
            }
        };

        struct Article{
            int id;
            std::string title;
            std::string author;
            std::string text;
        };


};

#endif