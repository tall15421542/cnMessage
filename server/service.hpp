#ifndef SERVICE_HEADER
#define SERVICE_HEADER

class Service{
    virtual void service(){};
    virtual void ackClient(){};
};

class FileService: public Service{
    void service() override;
    void ackClient() override;
};

class MsgService: public Service{
    void service() override;
    void ackClient() override;
};

class SignInService: public Service{
    void service() override;
    void ackClient() override;
};

class SignUpService: public Service{
    void service() override;
    void ackClient() override;
};
class SignOutService: public Service{
    void service() override;
    void ackClient() override;
};

class HistoryService: public Service{
    void service() override;
    void ackClient() override;
};


#endif // SERVICE_HEADER
