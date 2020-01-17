#ifndef SERVICE_HEADER
#define SERVICE_HEADER
#include "../message/message.hpp"
class Service{
    public:
    virtual void service(size_t socketFd, Message * msg){};
};

class FileService: public Service{
    public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient();
};

class MsgService: public Service{
    public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient(size_t socketFd, ChatAck ackStatus, ChatMsg * msg);
};

enum UserInfoStatus{
    USER_MATCH,
    USER_NOT_EXIST,
    USER_PASS_NOT_MATCH,
};

class SignInService: public Service{
    public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient(size_t socketFd, SignInAck ackStatus, SignInMsg * msg);
};


class SignUpService: public Service{
    public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient(size_t socketFd, SignUpAck ackStatus, SignUpMsg * msg);
};
class SignOutService: public Service{
    public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient(size_t socketFd, SignOutAck ackStatus, SignOutMsg * msg);
};

class HistoryService: public Service{
public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient();
};

class BuildDataConnService: public Service{
public:
    void service(size_t socketFd, Message * msg) override;
    void ackClient(size_t socketFd, BuildDataConnAck ackStatus, BuildDataConnMsg * msg);
};

#endif // SERVICE_HEADER
