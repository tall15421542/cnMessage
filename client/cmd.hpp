#ifndef CMD_HEADER
#define CMD_HEADER
#include "client.hpp"
#include "../message/message.hpp"
#include<map>
using namespace std;

class Cmd;
class SendCmd;
class SendHandler;
class SendMessageCmd;
class sendFileCmd;

class LoginCmd;
class LogoutCmd;
class SignUpCmd;
class RetrieveCmd; 

class Cmd{
  public:
    Cmd(){};
    ~Cmd();
    virtual void cmdExec(vector<string>& argv){ cout << "\n no it's base!";}
};

// send message/file
class SendCmd: public Cmd{
  public:
    SendCmd(){};
    ~SendCmd();
    void cmdExec(vector<string>& argv) override;
    map<const MsgType, SendHandler *> _handlerMap;
};

class SendHandler{
  public:
    SendHandler(){};
    ~SendHandler();
    void sendHeader(){};
    void sendContent(){};
};
class SendMessageCmd: public SendHandler{
  public:
    SendMessageCmd(){};
    ~SendMessageCmd();
    void sendHeader(); 
    void sendContent();
};

class SendFileCmd: public SendHandler{
  public:
    SendFileCmd(){};
    ~SendFileCmd();
    void sendHeader(); 
    void sendContent();
};

// login/logout/signUp
class LoginCmd: public Cmd{
  public:
    LoginCmd(){};
    ~LoginCmd();
    void cmdExec(vector<string> & argv) override;
};

class LogoutCmd: public Cmd{
  public:
    LogoutCmd(){};
    ~LogoutCmd();
    void cmdExec(vector<string> & argv) override;
};

class SignUpCmd: public Cmd{
  public:
    SignUpCmd(){};
    ~SignUpCmd();
    void cmdExec(vector<string> & argv) override; 
};

// retrieve
class RetrieveCmd: public Cmd{
  public:
    RetrieveCmd(){};
    ~RetrieveCmd();
    void cmdExec(vector<string> & argv) override; 
};

#endif // Cmd_HEADER
