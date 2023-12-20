#include "../include/diskServer.h"
#include "../include/protocol.h"
#include <algorithm>
#include <sys/stat.h>
#include<iostream>
#include<fstream>
#include <filesystem>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

diskServer::diskServer(){
    if (fs::exists(dir)){
        fs::current_path(dir);
        ifstream ng_is("infoNG.txt");
        ifstream a_is("infoA.txt");
        string ngIdS;
        while(ng_is >> ngId){
            getline(ng_is, ngIdS);
        }
        ngId++;
        string aIdS;
        int NGid;
        while(a_is >> NGid >> aId >> aIdS){
            getline(a_is, aIdS);
        }
        aId++;
    } else{
        fs::create_directory(dir);
        fs::current_path(dir);
        ofstream osNG("infoNG.txt");
        ofstream osA("infoA.txt");
        ngId = 1;
        aId = 1;
    }
 
}

void diskServer::listNewsgroups(MessageHandler& mh){
    ifstream is("infoNG.txt");

    string title;
    int id;
    int counter = 0;

    while(is >> id) {
        getline(is, title);
        counter++;
    }
    mh.sendIntParameter(counter);

    ifstream ngis("infoNG.txt");
    while(ngis >> id) {
        getline(ngis >> ws, title);
        mh.sendIntParameter(id);
        mh.sendStringParameter(title);
    }

}

bool diskServer::createNewsgroup(string title){
    ifstream is("infoNG.txt");

    string existingTitle;
    int id;

    while(is >> id) {
        getline(is, existingTitle);
        if(existingTitle == (" " + title)) {
            cout << "Finns redan";
            return false;
        }
    }

    int newId = ngId++;
    fs::create_directory(to_string(newId));

    //Lägg till den nya directoryn längst ner i info.txt
    ofstream os("infoNG.txt", ios_base::app);
    os << newId << " " << title << endl;
    os.close();    

    return true;
    
}
void diskServer::eraseFileLine(string path, string eraseLine) {
    string line;
    ifstream fin;
    
    fin.open(path);
    // contents of path must be copied to a temp file then
    // renamed back to the path file
    ofstream temp;
    temp.open("temp.txt");

    while (getline(fin, line)) {
        // write all lines to temp other than the line marked for erasing
        if (line != eraseLine) {
            temp << line << endl;
        }
    }

    temp.close();
    fin.close();

    // required conversion for remove and rename functions
    const char * p = path.c_str();
    remove(p);
    rename("temp.txt", p);
}

bool diskServer::deleteNewsgroup(int id){
    string title;
    int id1;
    ifstream is("infoNG.txt");

    while (is >> id1) {
        getline(is, title);
        if (id1 == id) {
            fs::remove_all(to_string(id));
            eraseFileLine("infoNG.txt", to_string(id) + title);
            return true;
        }
    }
    return false;
}


bool diskServer::listArticles(int NGid, MessageHandler& mh){

    ifstream is("infoA.txt");
    ifstream isNG("infoNG.txt");

    string title;
    int id;
    int NGid2;
    int nbrA = 0;
    bool ngexists = false;

    while(isNG >> NGid2) {
        getline(isNG, title);
        if(NGid2 == NGid) {
            ngexists = true;
        }
    }

    while(is >> NGid2 >> id){
        getline(is, title);
        if(NGid2 == NGid){
            nbrA++;
        }
    }
    
    if(!ngexists){
        return false;
    }

    mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
    mh.sendIntParameter(nbrA);

    ifstream isA("infoA.txt");
    while(isA >> NGid2 >> id) {
        getline(isA >> ws, title);
        if(NGid2 == NGid){
            mh.sendIntParameter(id);
            mh.sendStringParameter(title);
        }
    }
    return true;
    
}

bool diskServer::createArticle(int id, string title, string author, string text){
    ifstream is("infoNG.txt");
    ofstream os_a("infoA.txt", ios_base::app);

    string NGtitle;
    int NGid;

    while(is >> NGid){
        getline(is, NGtitle);
        if(id == NGid){
            fs::current_path("./" + to_string(NGid));
            ofstream os(to_string(aId) + ".txt");
            os << title << endl;
            os << author << endl;
            os << text << endl;

            os_a << NGid << " " << aId << " " << title << endl;
            os_a.close();    
            aId++;
            fs::current_path("..");
            return true;
        }
    }
    return false;
}

int diskServer::deleteArticle(int groupId, int articleId){
    string title;
    string NGtitle;
    int Aid;
    int NGid;
    ifstream is("infoA.txt");
    ifstream isNG("infoNG.txt");

    while(isNG >> NGid){
        getline(isNG, NGtitle);
        if(NGid == groupId){
             while (is >> NGid >> Aid) {
                getline(is, title);
                if(NGid == groupId && Aid == articleId){
                    eraseFileLine("infoA.txt", to_string(NGid) + " " + to_string(Aid) + title);
                    fs::current_path("./" + to_string(NGid));
                    fs::remove_all(to_string(Aid) + ".txt");
                    fs::current_path("..");
                    return 1;
                }
            }
            // article isn't found
            return 2;
        }
        
    }
    //NG isn't found
    return 3;
}

int diskServer::getArticle(int groupId, int articleId, MessageHandler& mh){
   
    string title;
    string NGtitle;
    int Aid;
    int NGid;
    ifstream is("infoA.txt");
    ifstream isNG("infoNG.txt");

    while(isNG >> NGid){
        getline(isNG, NGtitle);
        if(NGid == groupId){
             while (is >> NGid >> Aid) {
                getline(is, title);
                if(NGid == groupId && Aid == articleId){
                    fs::current_path("./" + to_string(NGid));
                    ifstream isA(to_string(articleId) + ".txt");
                    string ATitle;
                    string AAuthor;
                    string AText;
                    getline(isA, ATitle);
                    getline(isA, AAuthor);
                    string textAppend;
                    
                    getline(isA, AText);
                    textAppend.append(AText);
                    while(getline(isA, AText)) {
                        textAppend.append("\n");
                        textAppend.append(AText);
                    }
                    mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                    mh.sendStringParameter(ATitle);
                    mh.sendStringParameter(AAuthor);
                    mh.sendStringParameter(textAppend);
                    fs::current_path("..");
                    return 1;
                }
            }
            // article isn't found
            return 2;
        }
        
    }
    //NG isn't found
    return 3;
}
