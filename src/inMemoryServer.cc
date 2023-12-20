#include "../include/inMemoryServer.h"
#include "../include/protocol.h"
#include <algorithm>
#include <sys/stat.h>

using namespace std;

inMemoryServer::inMemoryServer(){
    ngId = 1;
    aId = 1;
}

void inMemoryServer::listNewsgroups(MessageHandler& mh) {
    
    //Antalet newsgroups
    mh.sendIntParameter(db.size());

    for ( const auto &p : db ) {
        mh.sendIntParameter(p.first.id);
        mh.sendStringParameter(p.first.name);
    }
}

bool inMemoryServer::createNewsgroup(string title){
   
    auto it = find_if(db.begin(), db.end(), [&title](std::pair<Newsgroup, std::vector<Article>> p){
        return p.first.name == title;
    });

    if(it != db.end()) {
        // Couldn't create newgroup as it already exist
        return false;
    }
    else { 
        db.insert({Newsgroup{ngId++, title}, vector<Article>()});
        return true;
    }
}

bool inMemoryServer::deleteNewsgroup(int id) {
    auto it = ng_exist_by_id(id);

    if(it == db.end()) {
        // Couldn't delete newgroup as it already exist
        return false;
    }
    else { 
        db.erase(it);
        return true;
    }

}

bool inMemoryServer::listArticles(int id, MessageHandler& mh) {
    auto it = ng_exist_by_id(id);

    if(it != db.end()){
        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
        mh.sendIntParameter(it->second.size());
        
        for(auto &a: it->second){
            mh.sendIntParameter(a.id);
            mh.sendStringParameter(a.title);
        }
        return true;
    }
    
    return false;
    
}

bool inMemoryServer::createArticle(int id, string title, string author, string text){

    auto it = ng_exist_by_id(id);

    if(it != db.end()){
        it->second.push_back(Article{aId++, title, author, text});
        return true;
    } else {
        return false;

    }
}

int inMemoryServer::deleteArticle(int groupId, int articleId){
    auto it = ng_exist_by_id(groupId);
    
    if (it != db.end()) {
        auto it2 = find_if(it->second.begin(), it->second.end(), [&articleId] (Article a) {
            return a.id == articleId;
        });
        if (it2 != it->second.end()) {
            it->second.erase(it2);
            return 1;
        }
        // If article isn't found
        else {
            return 2;
        }
    }
    // If newsgroup isn't found
    else {
        return 3;
    }
}


int inMemoryServer::getArticle(int groupId, int articleId, MessageHandler& mh) {
    auto it = ng_exist_by_id(groupId);
    
    if (it != db.end()) {
        auto it2 = find_if(it->second.begin(), it->second.end(), [&articleId] (Article a) {
            return a.id == articleId;
        });
        if (it2 != it->second.end()) {
            mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
            mh.sendStringParameter(it2->title);
            mh.sendStringParameter(it2->author);
            mh.sendStringParameter(it2->text);
            return 1;
        }
        else {
            return 2;
        }
    }
    else {
            return 3;
    }

}