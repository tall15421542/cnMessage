#include "client.hpp"
#include <cassert>
#include <cstring>
#include <cmath>
#include <iomanip>
#include "client.hpp"
#include "../socket/socketCommon.hpp"
using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);
void set_keypress();
void reset_keypress();
//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
Client::readCmd()
{
  readCmdInt(cin);
}

void
Client::printError(CMD_ERROR error, string cmd){
    switch(error){
        case(ILLEGAL_CMD):
            cout << "\nillegal command " << cmd << flush;
        default:
            break;
    }
}

void
Client::execCmd(vector<string> &cmdTokens){
    CmdMap::iterator it = _cmdExecMap.find(cmdTokens[0]);
    if(it == _cmdExecMap.end()){
        printError(ILLEGAL_CMD, cmdTokens[0]);
    }
    else{
        it->second->cmdExec(cmdTokens); 
    }
}

void 
Client::monitorMsg(){
}

void
Client::readCmdInt(istream& istr)
{
  ParseChar pch = getChar(istr);
  if (pch == INPUT_END_KEY) return;
  switch (pch) {
     case LINE_BEGIN_KEY :
     case HOME_KEY       : moveBufPtr(_readBuf); break;
     case LINE_END_KEY   :
     case END_KEY        : moveBufPtr(_readBufEnd); break;
     case BACK_SPACE_KEY : if(moveBufPtr(_readBufPtr - 1))deleteChar(); break;
     case DELETE_KEY     : deleteChar(); break;
     case NEWLINE_KEY    : {
                            vector<string> cmdTokens;
                            getCmdTokens(cmdTokens);
                            if(cmdTokens.size() > 0){
                                execCmd(cmdTokens);
                                addHistory();
                            }
                            cout << char(NEWLINE_KEY) << flush;
                            resetBufAndPrintPrompt(); 
                            break;
                           }
     case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
     case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
     case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
     case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
     case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
     case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
     case TAB_KEY        : {
                              completeCmd(); break;
                           }
     case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
     case UNDEFINED_KEY:   mybeep(); break;
     default:  // printable character
        insertChar(char(pch)); break;
  }
}

void Client::reprintCmd(){
    printPrompt();
    cout << string(_readBuf, _readBufEnd - _readBuf) << flush;
}

void Client::completeCmd(){
    vector<string> tokens;
    vector<string> samePrefixCmd;
    getCmdTokens(tokens);
    if(tokens.size() == 1){
        commonPrefix(samePrefixCmd, tokens[0]);
        if(samePrefixCmd.size() == 0){
            mybeep();
            return;
        }
        else if(samePrefixCmd.size() == 1){
           deleteLine();
           strcpy(_readBuf, samePrefixCmd[0].c_str());
           cout << _readBuf << flush;
           _readBufPtr = _readBufEnd = _readBuf + samePrefixCmd[0].size();
        }
        else{
            for(size_t idx = 0 ; idx < samePrefixCmd.size() ; ++idx){
                if(idx % 5 == 0 ) cout << endl;
                cout << setw(16) << left << samePrefixCmd[idx] << flush;
            }
            reprintCmd();
        }
    }
}


void 
Client::commonPrefix(vector<string>& cmds, string prefix){
    CmdMap::iterator it;
    for(it = _cmdExecMap.begin() ; it != _cmdExecMap.end() ; ++it){
        string cmd = it->first;
        bool hasSamePrefix = true;
        for(size_t i = 0 ; i < prefix.length() ; ++i){
            if(prefix[i] != cmd[i]) {hasSamePrefix = false; break;}
        }
        if(hasSamePrefix){
            cmds.push_back(cmd);
        }
    }
}


char * getNextToken(string & token, char * begin, char * end){
    int headIdx = -1;
    for(int i = 0 ; i < (end - begin) ; ++i){
        if(begin[i] != ' ') {headIdx = i; break;}
    }
    if(headIdx == -1){
        return end;
    }
    int tailIdx = (end - begin);
    for(int i = headIdx ; i <= (end - begin) ; ++i){
        if(begin[i] == ' ' || begin[i] == 0) {tailIdx = i; break;}
    }
    token = string(&begin[headIdx], tailIdx - headIdx);
    // clean tail space
    for(int i = tailIdx ; i <= (end - begin) ; ++i){
        if(begin[i] != ' ' || begin[i] == 0){
            tailIdx = i;
            break;
        }
    }
    return begin + tailIdx;
}
void
Client::getCmdTokens(vector<string>& tokens){
    string token;
    char * nextBegin = getNextToken(token, _readBuf, _readBufEnd);
    tokens.push_back(token);
    while(nextBegin != _readBufEnd){
        nextBegin = getNextToken(token, nextBegin, _readBufEnd);
        tokens.push_back(token);
    }
}


bool
Client::moveBufPtr(char* const ptr)
{
    if(ptr < _readBuf || ptr > _readBufEnd){
        mybeep();
        return false;
    }
    int movement = _readBufPtr - ptr;
    if(movement > 0){
        for(int i = 0 ; i < movement ; i++){
            cout << "\b" << flush;
        }
    }
    else{
        for(int i = 0 ; i < abs(movement) ; i++){
            cout << *(_readBufPtr + i) << flush;
        }
    }
    _readBufPtr = ptr;
    return true;
}


bool
Client::deleteChar()
{
    if(_readBufPtr == _readBufEnd){
        mybeep();
        return false;
    }
    for(char * ptr = _readBufPtr+1 ; ptr < _readBufEnd ; ptr++) {
        cout << *ptr;
        *(ptr - 1) = *ptr;
    }
    cout <<" \b";
    _readBufEnd --;
    *_readBufEnd = 0;
  
    char * pos = _readBufPtr;
    _readBufPtr = _readBufEnd;
    moveBufPtr(pos);
    return true;
}

void
Client::insertChar(char ch, int repeat)
{
    assert(repeat >= 1);
    
    for(int i = 0 ; i < repeat ; i++) cout << ch << flush;
    for(char * ptr = _readBufPtr ; ptr <= _readBufEnd ; ptr++) cout << *ptr << flush;
    
    char tmp[READ_BUF_SIZE];
    memcpy(tmp, _readBufPtr, _readBufEnd - _readBufPtr + 1);

    for(int i = 0 ; i < repeat ; i++) *(_readBufPtr++) = ch;
    _readBufEnd += repeat;
    memcpy(_readBufPtr, tmp, _readBufEnd - _readBufPtr + 1);
    *_readBufEnd = 0;

    char * pos = _readBufPtr;
    _readBufPtr = _readBufEnd;
    moveBufPtr(pos);
}

void
Client::deleteLine()
{
    int len = strlen(_readBuf);
    for(char * ptr = _readBufPtr ; ptr > _readBuf ; ptr--) cout << "\b" << flush;
    for(int i = 0 ; i < len ; i++) cout << " " << flush;    
    for(int i = 0 ; i < len ; i++) cout << "\b" << flush;  
    memset(_readBuf, 0, sizeof(char) * len);
    _readBufPtr = _readBufEnd = _readBuf;
    *_readBufEnd = 0;
}


void
Client::moveToHistory(int index)
{
    if(index < _historyIdx){
        if(_historyIdx == 0){
            mybeep();
            return;
        }
        if(_historyIdx == _history.size()){
            _history.push_back(_readBuf);
            _tempCmdStored = true;
        }
        if(index < 0) index = 0;
    }
    else if(index > _historyIdx){
        if((_tempCmdStored && _historyIdx == _history.size() - 1) ||
           (!_tempCmdStored && _historyIdx == _history.size() )){
            mybeep();
            return;
        }
        if(index >= _history.size()) index = _history.size() - 1;
    }
    else{
        mybeep();
        return;
    }
    _historyIdx = index;
    retrieveHistory();
    if(_tempCmdStored && _historyIdx == _history.size() - 1){
        _history.pop_back();
        _tempCmdStored = false;
    }
}


void
Client::addHistory()
{
    int head_idx = -1;
    for(int i = 0 ; i < strlen(_readBuf) ; i++){
        if(_readBuf[i]!=' '){
            head_idx = i;
            break;
        }
    }

    if(head_idx == -1){
        head_idx = _history.size();
        if(_tempCmdStored){
            _tempCmdStored = false;
            _history.pop_back();
            _historyIdx = _history.size();
        }
        return;
    }

    int tail_idx = -1; 
    for(int i = strlen(_readBuf) - 1; i >= 0 ; i--){
        if(_readBuf[i]!=' '){
            tail_idx = i;
            break;
        }
    }

    string buf(&_readBuf[head_idx], tail_idx - head_idx + 1);
    if(!_tempCmdStored) _history.push_back(buf);
    else{
        _history[_history.size() - 1] = _readBuf;
        _tempCmdStored = false;
    }
    
    _historyIdx = _history.size();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
Client::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf << flush;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}


size_t 
Client::buildDataConn(){
    _dataConnSocketFd = buildConnection();
    BuildDataConnMsg * msg = new BuildDataConnMsg(_userName);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(_dataConnSocketFd, msg, sizeof(BuildDataConnMsg), charAck);
    BuildDataConnMsg * ack = (BuildDataConnMsg *)charAck;
    if(ack->_buildDataConnAck == DATA_CONN_SUCCESS){
        cout << "Data socket is build" << endl;
        FD_SET(_dataConnSocketFd, &_readFdSet);
    }
    else{
        closeDataConn();
        cout << "Data socket is duplicated" << endl;
    }
    delete msg;
    return _dataConnSocketFd;
}

void 
Client::closeDataConn(){
    FD_CLR(_dataConnSocketFd, &_readFdSet);
    close(_dataConnSocketFd);
    _dataConnSocketFd = 0;
}

size_t max(size_t a, size_t b){
    if(a > b) return a;
    return b;
}

void
Client::run(){
    FD_ZERO(&_readFdSet);
    FD_SET(STDIN, &_readFdSet);
    FD_SET(_socketFd, &_readFdSet);
    resetBufAndPrintPrompt();
    char buf[MAX_MSG_SIZE + 40];
    size_t len;
    while(1){
        memset(buf, 0, sizeof(buf));
        fd_set working_read_set;
        memcpy(&working_read_set, &_readFdSet, sizeof(fd_set));
        int ret;
        set_keypress();
        if((ret = select(max(_socketFd, _dataConnSocketFd) + 1, &working_read_set, NULL, NULL, NULL) < 0)){
            perror("select error");
            exit(1);
        }
        reset_keypress();
        if(FD_ISSET(STDIN, &working_read_set)){
            readCmd();
        }
        else if(FD_ISSET(_socketFd, &working_read_set)){
            ChunkVec chunkVec;
            if((len =completeRead(_socketFd, buf, sizeof(buf), chunkVec)) == 0){
                cout << "\nServer is disconnected" << endl;
                exit(1);
            }
        }
        else if(_clientState != IDLE_CLIENT && FD_ISSET(_dataConnSocketFd, &working_read_set)){
            ChunkVec chunkVec;
            if((len =completeRead(_dataConnSocketFd, buf, sizeof(buf), chunkVec)) == 0){
                cout << "\nServer is disconnected" << endl;
                exit(1);
            }
            Message * firstPacket = (Message *)chunkVec[0]->_data;
            MsgType msgType = firstPacket->getMsgType();
            if(msgType == SEND_MSG){
                ChatMsg * chatMsg = (ChatMsg *)firstPacket;
                _messageRecord.push_back(chatMsg);
                if(chatMsg->_header._sender == _personTalking){
                    cout << "\n#####################################" << endl;
                    cout <<  _personTalking << ": " << string(reinterpret_cast<char *>(chatMsg->_buf)) << endl;
                    cout << "######################################" << endl;
                }
                else{
                    cout << "\n#####################################" << endl;
                    cout << "have new message from " << chatMsg->_header._sender << endl;
                    cout << "######################################" << endl;
                }
            }
            reprintCmd();
        }
        else{
            cout << "\nUnhandled case" << endl;
            exit(1);
        }
    }
}

