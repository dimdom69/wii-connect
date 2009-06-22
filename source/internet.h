
/*
 *      http.h
 *      
 *      Copyright 2009 Aaron Lehrian <jsmaster@tampabay.rr.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include <stdlib.h>

#define MAX_SOCKETS 99

	struct emsg{
		char *from;
		char *to;
		char *subject;
		char *message;
		
	};	
	
	
class internet{
	public:
		internet();
		~internet();
		void connect(const char *ipc,u32 ip = NULL, int port = 80, int snum = 1);
		int getnetstate();
		const char *getstate();
		void writetosocket(const char *write,const int snum = 1);
		char *readfromsocket(int bufsize = 1025,const int snum = 1);
		char *read(int bufsize = 1025,int snum = 1);
	protected:
		char localip[16];
		char gateway[16];
		char netmask[16];
		s32 netstate;
	
	private:
		int offset;
		int red;
		char *buff;
		s32 socket[MAX_SOCKETS];
	
};

class http : public internet{

public:
http();
~http();
char *gethttpfile(const char *file,char *host = NULL,int bufsize = 1025, int snum = 1);
protected:

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
}; 


class chat : public internet{
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

typedef struct messlist_s{
	char *subject;
	char *from;
	char *to;
	char *cc;
	char *date;
	char *data;
	int newmail;
	int important;
	int starred;
}messlist;

typedef struct mlist_s{
	int amount;
	messlist *ml;
}mlist;
class email : public internet{
	

public:
	
	email(char *ip,int port = 25);
	~email();
	void sendemail(struct emsg mess);
	mlist *getallmail(); //must have set settings first; gets new mail + saved mail
	

private:
	char *response;
	char *line;
	char *host;
	
	
	
	
	
};
/*
class network : public internet{
public:
	network();
	~network();
	void connect(char *a,u32 b = NULL, int c = 80, int d = 1);	
	char *gethttpfile(char *a,char *h = NULL,int b = 1025, int c = 1);
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
	char *host;
	s32 netstate;
	char *redfromsocket;
	const char *gotnetstate;
	char *gothttpfile;
	http *net;
	int all;
protected:
	
private:

};
*/
