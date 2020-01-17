#ifndef MSG_HEADER
#define MSG_HEADER
#include<string>
#include<bsd/md5.h>
#include<cstring>
#include<iostream>

using namespace std;
enum MsgType{
    SEND_MSG,
    SEND_FILE,
    SIGNIN,
    SIGNUP,
    SIGNOUT,
    HISTORY,
    BUILD_DATA_CONN,
};


// util
void md5(const char* str, size_t len, uint8_t digest[MD5_DIGEST_LENGTH]);
#define MAX_MSG_SIZE 4096
#define PAY_LOAD_LIMIT MAX_MSG_SIZE - sizeof(Header)
#define MAX_FILENAME_LEN 40
#define USER_NAME_LENGTH 30

class Chunk{
    public:
        Chunk(char * data, size_t size):_data(data), _size(size){ cout << "Hello" << endl;}
        ~Chunk(){delete[] _data;}
        char * _data;
        size_t _size;
};

class Header{
  public:
    char _sender[USER_NAME_LENGTH];
    char _receiver[USER_NAME_LENGTH];
    time_t _msgTime;
    size_t _startLength;
    size_t _contentLength;
    size_t _completeLength;
    MsgType _msgType;
    bool _isAck;
};
class Message{
    public:
      virtual ~Message(){};
      Header _header;
      bool completePacketDelivered(){
        return _header._completeLength == (_header._startLength + _header._contentLength);
      }

      void makeHeader(string sender, string receiver, MsgType type, size_t size){
        strcpy(_header._sender, sender.c_str());
        strcpy(_header._receiver, receiver.c_str());
        time(&_header._msgTime);
        _header._startLength = 0;
        _header._contentLength = size;
        _header._completeLength = size;
        _header._msgType = type;
        _header._isAck = false;
      }
      
      void md5(const char* str, size_t len, uint8_t digest[MD5_DIGEST_LENGTH]) {
        MD5_CTX ctx;
        MD5Init(&ctx);
        MD5Update(&ctx, (const uint8_t*)str, len);
        MD5Final(digest, &ctx);
      }
      MsgType getMsgType(){return _header._msgType;} 
      size_t getMsgSize(){return sizeof(_header) + payloadSize();}
      virtual size_t payloadSize(){ return 0; }
};

enum BuildDataConnAck{
    DATA_CONN_DUP,
    DATA_CONN_SUCCESS,
    DATA_CONN_FAIL,
};

class BuildDataConnMsg: public Message{
    public:
    BuildDataConnMsg(string userName){
        makeHeader("", "", BUILD_DATA_CONN, payloadSize());
        memset(_userName, 0, sizeof(_userName));
        memcpy(_userName, userName.c_str(), userName.length());
    }
    BuildDataConnAck _buildDataConnAck;
    unsigned char _userName[USER_NAME_LENGTH];
    size_t payloadSize(){return sizeof(BuildDataConnMsg) - sizeof(Header);}
};   

enum ChatAck{
    CHAT_SENT,
    CHAT_REICEIER_NOT_EXIST,
};
class ChatMsg: public Message{
    public:
      ChatMsg(){}
      ChatMsg(string sender, string receiver, string msg){
        memset(_buf, 0, sizeof(_buf));
        makeHeader(sender, receiver, SEND_MSG, msg.length());
        packData(msg);
      }
      size_t payloadSize() override{ return sizeof(ChatMsg) - sizeof(Header);}
      void packData(string msg){
        memcpy(_buf, msg.c_str(), msg.length());
      };
      ChatAck _chatAck;
      unsigned char _buf[MAX_MSG_SIZE - sizeof(Header) - sizeof(ChatAck)];
};

class FileMsg: public Message{
    public:
      size_t payloadSize() override{ return 1; }
      unsigned char _fileName[MAX_FILENAME_LEN];
      unsigned char _data[MAX_MSG_SIZE - MAX_FILENAME_LEN - sizeof(Header)];
};

enum SignInAck{
    SIGNIN_ACK_SUCCESS,
    SIGNIN_ACK_USER_NOT_EXIST,
    SIGNIN_ACK_PASS_NOT_MATCH,
};
class SignInMsg: public Message{
    public:
        SignInMsg(string userName, string password){
            memset(_userName, 0, sizeof(_userName));
            memset(_password, 0, sizeof(_password));
            makeHeader("", "", SIGNIN, payloadSize());
            packData(userName, password);
        }
        void packData(string userName, string password){
            memcpy(_userName, userName.c_str(), userName.length());
            md5(password.c_str() , password.length(), _password);
        }
        size_t payloadSize() override{return sizeof(SignInMsg) - sizeof(Header);}
        SignInAck _signInAck;
        unsigned char _userName[USER_NAME_LENGTH];
        unsigned char _password[MD5_DIGEST_LENGTH];
};

enum SignOutAck{
    SIGNOUT_ACK_SUCCESS,
};

class SignOutMsg: public Message{
    public:
      SignOutMsg(string userName){
        memset(_userName, 0, sizeof(_userName));
        makeHeader("", "", SIGNOUT, payloadSize());
        packData(userName);
      }
      void packData(string userName){
        memcpy(_userName, userName.c_str(), userName.length());
      }
      size_t payloadSize() override{return sizeof(SignOutMsg) - sizeof(Header);}
      SignOutAck _signOutAck;
      unsigned char _userName[USER_NAME_LENGTH];
};

enum SignUpAck{
    SIGNUP_ACK_SUCCESS,
    SIGNUP_ACK_FAIL,
};
class SignUpMsg: public Message{
    public:
        SignUpMsg(string userName, string password){
            memset(_userName, 0, sizeof(_userName));
            memset(_password, 0, sizeof(_password));
            makeHeader("", "", SIGNUP, payloadSize());
            packData(userName, password);
        }
        size_t payloadSize() override{ return sizeof(SignUpMsg) - sizeof(Header); }
        void packData(string userName, string password){
            memcpy(_userName, userName.c_str(), userName.length());
            md5(password.c_str() , password.length(), _password);
        }
        SignUpAck _signUpAck;
        unsigned char _userName[USER_NAME_LENGTH];
        unsigned char _password[MD5_DIGEST_LENGTH];
};

#endif // MSG_HEADER 
