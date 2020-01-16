#ifndef CLIENT_HEADER
#define CLIENT_HEADER
#include<map>
#include<iostream>
#include<vector>
#include<string>
#include "cmd.hpp"
#include "cmdCharDef.h"
using namespace std;
class Client;

extern Client * g_client;
enum ClientState{
  IDLE,
  LOGIN,
  TALKING,
};

enum CMD_ERROR{
    ILLEGAL_CMD,
};
typedef map<const string, Cmd*> CmdMap;
typedef pair<const string, Cmd*>  CmdRegPair;
class Client{
#define READ_BUF_SIZE    65536
#define TAB_POSITION     8
#define PG_OFFSET        10
#define USER_NAME_LENGTH 30
  public:
    Client(): _readBufPtr(_readBuf), _readBufEnd(_readBuf), _historyIdx(0), _tempCmdStored(false),
              _clientState(IDLE), _userName("Unknown"), _personTalking("NOBODY")
    {
      _cmdExecMap.insert(CmdRegPair("send", new SendCmd));
      _cmdExecMap.insert(CmdRegPair("login", new LoginCmd));
      _cmdExecMap.insert(CmdRegPair("logout", new LogoutCmd));
      _cmdExecMap.insert(CmdRegPair("register", new RegisterCmd));
      _cmdExecMap.insert(CmdRegPair("retrieve", new RetrieveCmd));
    }
    void resetBufAndPrintPrompt() {
        _readBufPtr = _readBufEnd = _readBuf;
        *_readBufPtr = 0;
        printPrompt();
    }
    void readCmd();
    void readCmdInt(istream&);
    void printPrompt() const { cout << this->_userName << "> "; }
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
    string _userName;
    string _personTalking;
};


#endif // CLIENT_HEADER
