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
};
class Message{
    public:
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
      }
      
      void md5(const char* str, size_t len, uint8_t digest[MD5_DIGEST_LENGTH]) {
        MD5_CTX ctx;
        MD5Init(&ctx);
        MD5Update(&ctx, (const uint8_t*)str, len);
        MD5Final(digest, &ctx);
      }
      
      size_t getMsgSize(){return sizeof(_header) + payloadSize();}
      virtual size_t payloadSize(){ return 0; }
};

class ChatMsg: public Message{
    public:
      ChatMsg(string sender, string receiver, string msg){
        makeHeader(sender, receiver, SEND_MSG, msg.length());
      }
      size_t payloadSize() override{ return 1;}
      unsigned char _buf[MAX_MSG_SIZE - sizeof(Header)];
};

class FileMsg: public Message{
    public:
      size_t payloadSize() override{ return 1; }
      unsigned char _fileName[MAX_FILENAME_LEN];
      unsigned char _data[MAX_MSG_SIZE - MAX_FILENAME_LEN - sizeof(Header)];
};

class SignInMsg: public Message{
    public:
      size_t payloadSize() override{return 1;}
        unsigned char _userName[USER_NAME_LENGTH];
        unsigned char _password[MD5_DIGEST_LENGTH];
};

class SignOutMsg: public Message{
    public:
      size_t payloadSize() override{return 1;}
        unsigned char _userName[USER_NAME_LENGTH];
};

class SignUpMsg: public Message{
    public:
        SignUpMsg(string userName, string password){
            makeHeader("", "", SIGNUP, payloadSize());
        }
        size_t payloadSize() override{ return sizeof(_userName) + sizeof(_password); }
        void packData(string userName, string password){
            memcpy(_userName, userName.c_str(), userName.length());
            md5(password.c_str() , password.length(), _password);
        }
        unsigned char _userName[USER_NAME_LENGTH];
        unsigned char _password[MD5_DIGEST_LENGTH];
};

#endif // MSG_HEADER 
