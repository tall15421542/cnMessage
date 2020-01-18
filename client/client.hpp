#ifndef CLIENT_HEADER
#define CLIENT_HEADER
#include<map>
#include<iostream>
#include<vector>
#include<string>
#include<thread>
#include "cmd.hpp"
#include "cmdCharDef.h"
#include "../socket/socketClient.hpp"
using namespace std;
class Client;

extern Client * g_client;
enum ClientState{
  IDLE_CLIENT,
  SIGNIN_CLIENT,
  TALKING_CLIENT,
};

enum CMD_ERROR{
    ILLEGAL_CMD,
};
typedef map<const string, Cmd*> CmdMap;
typedef pair<const string, Cmd*>  CmdRegPair;
typedef map<string, vector<Message*> *> MsgRecordMap;
typedef pair<string, vector<Message*> *> MsgRecordMapPair;
class Client{
#define READ_BUF_SIZE    65536
#define TAB_POSITION     8
#define PG_OFFSET        10
#define STDIN            0
    public:
    Client(): _readBufPtr(_readBuf), _readBufEnd(_readBuf), _historyIdx(0), _tempCmdStored(false),
              _clientState(IDLE_CLIENT), _userName("Unknown"), _personTalking("NOBODY"), _dataConnSocketFd(0)
    {
      _cmdExecMap.insert(CmdRegPair("send", new SendCmd));
      _cmdExecMap.insert(CmdRegPair("signIn", new SignInCmd));
      _cmdExecMap.insert(CmdRegPair("signOut", new SignOutCmd));
      _cmdExecMap.insert(CmdRegPair("signUp", new SignUpCmd));
      _cmdExecMap.insert(CmdRegPair("retrieve", new RetrieveCmd));
      _cmdExecMap.insert(CmdRegPair("update", new UpdateCmd)); 
      _cmdExecMap.insert(CmdRegPair("read", new ReadCmd));
      _socketFd = buildConnection();
    }
    void run();
    void resetBufAndPrintPrompt() {
        _readBufPtr = _readBufEnd = _readBuf;
        *_readBufPtr = 0;
        printPrompt();
    }
    void readCmd();
    void readCmdInt(istream&);
    void printPrompt() const { 
        cout << this->_userName << flush;
        if(_clientState == TALKING_CLIENT){
            cout << "(Talking to " << _personTalking << ")" << flush;
        }
        cout << ">" << flush;
    }
    bool moveBufPtr(char* const);
    bool deleteChar();
    void insertChar(char, int = 1);
    void deleteLine();
    void moveToHistory(int index);
    void addHistory();
    void retrieveHistory();
    
    // for tab
    void commonPrefix(vector<string>& tokens, string prefix); 
    void getCmdTokens(vector<string>& tokens);
    void reprintCmd();
    void completeCmd();

    // for cmdExec
    void printError(CMD_ERROR error, string cmd);
    void execCmd(vector<string> &);
   
    // for monitor message from server
    size_t buildDataConn();
    void closeDataConn();
    void monitorMsg();

    fd_set _readFdSet;
    fd_set _writeFdSet;
    // for read cmd 
    char      _readBuf[READ_BUF_SIZE];
    char*     _readBufPtr;
    char*     _readBufEnd;
    vector<string>   _history;
    int              _historyIdx;
    bool      _tempCmdStored;

    // map<char* , Cmd*> _cmdExecMap;
    map<string , vector<char*> *> _messageStore;
    map<string , Cmd*> _cmdExecMap;
    ClientState _clientState;
    
    // for msg
    size_t _socketFd;
    string _userName;
    string _personTalking;

    // for data
    size_t _dataConnSocketFd;
    vector<Message *> _messageRecord;
    MsgRecordMap _msgRecordMap;
};


#endif // CLIENT_HEADER
