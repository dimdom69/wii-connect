#include <stdlib.h>

#define MAX_SOCKETS 99

	struct emsg{
		char *from;
		char *to;
		char *subject;
		char *message;
		
	};	

class http{

public:
http();
~http();
void connect(const char *ipc,u32 ip = NULL, int port = 80, int snum = 1);
char *gethttpfile(const char *file,int bufsize = 1025, int snum = 1);
const char *getstate();
void writetosocket(const char *write,const int snum = 1);
char *readfromsocket(int bufsize = 1025,const int snum = 1);
char *read(int bufsize = 1025,int snum = 1);
s32 netstate;

protected:
char localip[16];
char gateway[16];
char netmask[16];

private:
struct httpresponse{
  float version;
  int response_code;
  char *text;
  char *date;
  char *modified;
  char *server;
  size_t content_length;
  char *content_type;
  char *charset;
 
};
char *httpget;
int offset;
int red;
char *buff;
s32 socket[MAX_SOCKETS];
}; 


class chat : public http{
public:
	chat();
	~chat();
	void chat_keepalive();
	void chat_connect();
	void chat_disconnect();
	void chat_sendmessage(char *message,int dest);
	char *chat_getmessages();
protected:

private:
	char *mesbuf;
	char *status;
};


class email : public http{
	

public:
	
	email(char *ip,int port = 25);
	~email();
	void sendemail(struct emsg mess);
	

private:
	char *response;
	char *line;
	char *host;
	
	
	
	
	
};

class network : public http{
public:
	network();
	~network();
	void connect(char *a,u32 b = NULL, int c = 80, int d = 1);	
	char *gethttpfile(char *a,int b = 1025, int c = 1);
	const char *getstate();
	void writetosocket( char *a,int b = 1);
	char *readfromsocket(int a = 1025,int b = 1);
	char *file;
	char *write;
	int bufsize;
	int snum;
	char *ipc;
	u32 ip;
	int port;
	s32 netstate;
	char *redfromsocket;
	const char *gotnetstate;
	char *gothttpfile;
	http *net;
	int all;
protected:
	
private:

};
