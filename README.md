# cnMessage
## User Guide
My user interface is friendly to the Computer science students because it works like a command-line tool. My simplified version supports the tab complete, move up or down to see the command history. Backward, left and right key are also supported to manipulate the command.

Client side support 6 command
* signUp
* signIn
* signOut
* send
* update
* read

### signUp
```
Usage:
[username]> signUp
Please Enter the userName:
Please Enter the password:
```
#### arg: 
NONE
#### desription
If the ``userName`` has existed, user would receive a duplicate error. The password is encrypt using MD5;

### signIn
```
Usage: 
[username]> signIn
Please Enter the userName:
Please Enter the password:
```
#### arg: 
None
#### description
The non-existed username or the password mismatch would have their error message. If the signUp request is approved, the name on the left side would display the authenticated username.

if the users have unread messages during the offline time, the server would send the unread messages to them once they successfully signIn.

### signOut
```
Usage:
[username]> signOut
```
#### arg: 
NONE
#### description
SignOut the account.

### send
```
Usage:
[username]> send [receiver]
[username](Taling to [receiver])> send
```
#### arg:
[receiver]

#### description
The client can use this command to send messages. If the userName does not exist, the server would respond to client an error message. If the user does not chat with anyone else in this connection, the receiver argument is required. Once the user chooses a person to chat, the receiver is not required, unless the user wants to switch the person to talk.

I only show the message contents from the current session, messages from another user would be a simple tag to notify the user. Users can use ``read [username]`` to check the inbox message.

### Read
```
Usage:
[username]> read -l
[username]> read [username1] [username2] [username3]...
```
#### arg
[-l] 
[username1] [username2] [username3]...
#### description
``read`` can help user to see the received message. User can use ``[-1]`` option to see the user list who chat with user in this connection.

If the client needs the whole view of messages history, the ``update`` command is required.

### update
```
Usage:
[username]> update
```
#### arg
None
#### description
``update`` would request the complete history from the server. Once ``update`` is done, client can use ``read`` command described above to view the messages.

## Instructions on how to run server &amp; clients

### For Server
```
// Under server/ directory
make
```

### For Client
```
// Under client/ directory
make
```

### Specify Port
```c 
// Under socket/socketCommon.hpp
// Line 6
#define SERVER_PORT 3000 
```
## System & Program Design
### Message
![](https://i.imgur.com/JHDDVvR.png)

There are lots of different kinds of requests, and the service is subjected to change over time. Therefore, I abstract a ``Message`` class to cooperate with the communication between the client and the server. Every specific message class is inherited from the base class ``Message``.  

### CmdExec(Client) / Service(Server)
![](https://i.imgur.com/cAzqV5c.png)
 
 
As previously described, the message has lots of kinds, so do the client CmdExec and server Service. Deploying a similar logic, I abstract a ``service`` and ``cmdExec`` class. I use a ``ServiceMgr`` to dispatch requests to the corresponding service. I use ``cmdMgr`` to parse the user request and dispatch request to the ``CmdExec`` 

### Open/Close principle
![](https://i.imgur.com/siblICM.png)

Using the architecture described above, the most significant advantage is that once I have implemented the communication mechanism for the server and the client, the implementation of a new service is simply to add a new derived class of ``Message``, ``cmdExec`` and ``service``. It's open for extension and close for modification.

Except for the big picture of architecture, there are two issues I want to discuss here. First, how to decode the socket stream on the receiver side. Second, how to handle the unexpected message from other clients. 

### How to decode the socket stream on the other side
The communication of the client and the server is through the TCP socket. We need a mechanism to identify the different kinds of messages received in the char type buffer. On the sender side, to achieve the message class transparency, we always use the base class pointer to access the message, and each type of message has a different size. We also need a mechanism to know how many bytes to send a message in the socket API.

My solution is to design a Header data structure in the base class. Before each access to the message, I always cast the message pointer into ``Message *``. On the receiver side, I use the information stored in the header to dispatch the message to the right service handler. On the sender side, I put size and type information when a message object is created.

### Unexpected message from the server
There are two kinds of messages in the application, one is the control message, the other is the data message.

The control message is predictable. Like the ack from the server after the ``signIn`` request is issued. The client-side is expected to receive the ack and would block until the ack is received.

The data message is unpredictable. Like the realtime message from the other client. 

Therefore, if I use only one socket to listen to the messages from the server, each block client needs to handle the unexpected message, which violates my design of class transparency, and it's a little annoying when I add a new class of service.

To simplify the handling process, I use two sockets to listen to messages from the server. One is for a control message, the other is for unexpected data messages.

On the client-side, we have to monitor two input sources, one is ``STDIN`` to process user command, the other is input socket files to deal with messages from the server. To avoid busy waiting and polling, I use ``select`` function to monitor the file descriptors, and I add the ``STDIN``, ``control_socker``, ``data_socket`` into ``_readFdSet``.

## Summary
I try to implement a web framework using C++, the design objective is to make the system open for extension and close for modification. I abstract the ``Cmd``, ``Message`` and ``Service`` class, the advantage of this design is that once I need to add a service like ``sendFile``, I only need to focus on implementing the corresponding derived class, it's simple and reliable to assure the tested service won't be destroyed by the poor implementation of the new service.

Look into the implementation detail, I talk about two difficulties here, one is socket stream decoding and the other is the unexpected data message.

To deal with the stream decoding, I store the header in the base classes and check the ``Message *``cast pointer in a consistent way.

To deal with the data message, I use two sockets, one is for a control message, the other one is for data message. With this design, we can simplify the handling flow for the service. For monitoring the ``STDIN`` and socket fds, I use ``select`` to implement the efficient multiplexing. 

## Appendix
To implement a command-line style interface, I need to process every char. However, the original setting of the terminal is line-buffered. To avoid the line buffer, I have to change the setting of Terminal and reset it when the command is parsed.
```cpp=
static struct termios stored_settings;

static void reset_keypress(void)
{
   tcsetattr(0,TCSANOW,&stored_settings);
}

static void set_keypress(void)
{
   struct termios new_settings;
   tcgetattr(0,&stored_settings);
   new_settings = stored_settings;
   new_settings.c_lflag &= (~ICANON);
   new_settings.c_lflag &= (~ECHO);
   new_settings.c_cc[VTIME] = 0;
   tcgetattr(0,&stored_settings);
   new_settings.c_cc[VMIN] = 1;
   tcsetattr(0,TCSANOW,&new_settings);
}

```
