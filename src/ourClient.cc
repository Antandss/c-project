/* myclient.cc: sample client program */
#include "../include/connection.h"
#include "../include/connectionclosedexception.h"
#include "../include/messageHandler.h"
#include "../include/protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;


/* Creates a client for the given args, if possible.
 * Otherwise exits with error code.
 */
Connection init(int argc, char* argv[])
{
        if (argc != 3) {
                cerr << "Usage: myclient host-name port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[2]);
        } catch (std::exception& e) {
                cerr << "Wrong port number. " << e.what() << endl;
                exit(2);
        }

        Connection conn(argv[1], port);
        if (!conn.isConnected()) {
                cerr << "Connection attempt failed" << endl;
                exit(3);
        }

        return conn;
}

int app(Connection &conn)
{
        MessageHandler mh(&conn);

        cout << "\n Choose a number: \n 1. List newsgroup \n 2. Create newsgroup \n 3. Delete newsgroup \n 4. List article \n 5. Create article \n 6. Delete article \n 7. Get article \n" << endl;
        int choice;
        while(cin >> choice) {
            try {
                
                switch(choice) {
                    case 1: {
                        mh.sendCode(static_cast<int>(Protocol::COM_LIST_NG));
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int recv = mh.recvCode();
                        if(recv == static_cast<int>(Protocol::ANS_LIST_NG)) {
                            int nbrNgs = mh.recvIntParameter();
                            cout << "There are " << nbrNgs << " newsgroups" << endl;
                            for (int i = 0; i < nbrNgs; ++i) {
                                cout << "ID: " << mh.recvIntParameter();
                                cout << ", Title: " << mh.recvStringParameter() << endl;
                            }             
                        }
                        else {
                            cout << "did not receive correct reply protocol, terminating.";
                            return 1;
                        }
                        mh.recvCode(); //END
                        break;
                    }
                    case 2:
                    {
                        mh.sendCode(static_cast<int>(Protocol::COM_CREATE_NG));
                        cout<<"Title of the newsgroup: "<<endl;
                        string title;
                        cin.ignore();
                        getline(cin, title);
                        mh.sendStringParameter(title);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        //ANS_CREATE_NG
                        mh.recvCode();
                        //Reply is ACK or NAK
                        int reply = mh.recvCode();
                        if(reply == static_cast<int>(Protocol::ANS_ACK)) {
                            cout << "Success, fully created" << endl;
                        }
                        else if(reply == static_cast<int>(Protocol::ANS_NAK)) {
                            //ERR NG ALREADY EXISTS
                            mh.recvCode();
                            cout << "Error: newsgroup already exists" << endl;
                        }
                        //ANS_END
                        mh.recvCode();
                        break;
                    }
                    case 3: {
                        mh.sendCode(static_cast<int>(Protocol::COM_DELETE_NG));
                        cout<<"ID of the newsgroup: "<<endl;
                        int id;
                        cin >> id;
                        mh.sendIntParameter(id);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        mh.recvCode(); //ANS_DELETE_NG
                        int code = mh.recvCode();

                        if (code == static_cast<int>(Protocol::ANS_ACK)) {
                            cout << "Newsgroup deleted, fully succesful" << endl;
                        }
                        else {
                            mh.recvCode(); //ERR_NG_NOT
                            cout << "Newsgroup does not exist" << endl;

                        }
                        mh.recvCode(); //END
                        break;
                    }
                    case 4: {
                        mh.sendCode(static_cast<int>(Protocol::COM_LIST_ART));
                        cout<<"ID of the newsgroup: "<<endl;
                        int id;
                        cin >> id;
                        mh.sendIntParameter(id);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));

                        mh.recvCode(); //ANS_LIST_ART
                        int code = mh.recvCode(); //ACK or NACK
                        if (code == static_cast<int>(Protocol::ANS_ACK)){
                            int nbrArts = mh.recvIntParameter();
                            cout << "Number of articles: " << nbrArts << endl;
                            for (int i = 0; i < nbrArts; i++) {
                                cout << "ID: " << mh.recvIntParameter();
                                cout << ", Title: " << mh.recvStringParameter() << endl;
                            }
                            
                        }
                        else {
                            mh.recvCode(); //ERR_NG_NOT_EXIST
                            
                            cout << "Newsgroup does not exist" << endl;
                        }
                        mh.recvCode(); //END
                        break;
                    }
                    case 5: {
                        mh.sendCode(static_cast<int>(Protocol::COM_CREATE_ART));
                        cout<<"ID of the newsgroup: "<<endl;
                        int id;
                        cin >> id;
                        cout<<"Title of the article: "<<endl;
                        cin.ignore();
                        string title;
                        getline(cin, title);
                        cout<<"Author of the article: "<<endl;
                        string author;
                        getline(cin, author);
                        cout<<"Write the article: "<<endl;
                        string text;
                        getline(cin, text);
                        mh.sendIntParameter(id);
                        mh.sendStringParameter(title);
                        mh.sendStringParameter(author);
                        mh.sendStringParameter(text);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));

                        mh.recvCode(); //ANS_CREATE_ART
                        int code = mh.recvCode(); //ANS AK OR NAK

                        if (code == static_cast<int>(Protocol::ANS_ACK)) {
                            cout << "Article succesfully created" << endl;
                        }
                        else {
                            mh.recvCode(); //ERR_NG_DOES_NOT_EZISTST
                            cout << "Newgroup doesn't exist :(" << endl;

                        }
                        mh.recvCode(); //END
                        break;
                    }
                    case 6: {
                        mh.sendCode(static_cast<int>(Protocol::COM_DELETE_ART));
                        cout << "Enter newsgroup ID: " << endl;
                        int groupId;
                        cin >> groupId;
                        cout << "Enter article ID: " << endl;
                        int artId;
                        cin >> artId;
                        mh.sendIntParameter(groupId);
                        mh.sendIntParameter(artId);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));

                        mh.recvCode(); //ANS_DELETE_ART
                        int code = mh.recvCode(); //AKK OR NAK
                        if (code == static_cast<int>(Protocol::ANS_ACK)){
                            cout << "Article succesfully deleted" << endl;
                        }
                        else {
                            int reply = mh.recvCode(); // NG OR ART DOES NOT EXIST
                            if (reply == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)) {
                                cout << "Article does not exist" << endl;
                            }
                            else {
                                cout << "Newgroup does not exist" << endl;
                            }
                        }
                        mh.recvCode(); //END
                        break;
                    }
                    case 7: {
                        mh.sendCode(static_cast<int>(Protocol::COM_GET_ART));
                        cout << "Enter newgroups ID: " << endl;
                        int groupid;
                        cin >> groupid;
            
                        cout << "Enter article ID: " << endl;
                        int artId;
                        cin >> artId;
                        
                        mh.sendIntParameter(groupid);
                        mh.sendIntParameter(artId);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));

                        mh.recvCode(); //ANS GET ART
                        int code = mh.recvCode(); //ACK OR NAK

                        if(code == static_cast<int>(Protocol::ANS_ACK)) {
                            cout << "Title: " << mh.recvStringParameter();
                            cout << ", Author: " << mh.recvStringParameter() << endl;
                            cout << "Text: " << mh.recvStringParameter();
                        }
                        else {
                            int recv = mh.recvCode();
                            if(recv == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)) {
                                cout << "Article does not exist" << endl;
                            }
                            else {
                                cout << "Newsgroup does not exist" << endl;
                            }
                        }
                        mh.recvCode(); //END
                        break;
                    }
                    }
            }
            catch(ConnectionClosedException&) {
                    cout << "Fel" << endl;

            }
            cout << "\n Choose a number: \n 1. List newsgroup \n 2. Create newsgroup \n 3. Delete newsgroup \n 4. List article \n 5. Create article \n 6. Delete article \n 7. Get article \n" << endl;
        }
        return 0;
}

int main(int argc, char* argv[])
{
        Connection conn = init(argc, argv);
        return app(conn);
}
