
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
#include <network.h>

#define POP_TO "To:"
#define POP_FROM "From:"
#define POP_CC "Cc:"
#define POP_DATE "Date:"
#define POP_SUBJECT "Subject:"
#define POP_BODY "\r\n\r\n"



using namespace std;



typedef struct mailsettings_s{
	char server[50];
	int port;
	char user[50];
	char password[50];
}mailsettings;
typedef struct messlist_s{
	char *subject;
	char *from;
	char *to;
	char *cc;
	char *date;
	char *body;
	int newmail;
	int important;
	int starred;
	
	messlist_s *next;
	
}messlist;

struct emsg{
	char from[50];
	char to[50];
	char subject[50];
	char message[200];	
};	

typedef struct _messdata{
	int num;
	int size;
}messdata;

enum settype{
	SMTP,
	POP,
	IMAP
};

enum net_protocol{
	UDP,
	TCP
};







	
	
class internet{
	public:
		internet();
		~internet();
		void connect(char *server, int port,net_protocol protocol);
		int getnetstate();
		const char *getstate();
		void writetosocket(const char *write);
		char *readfromsocket(int bufsize = 1025);
		char *read(int bufsize = 1025);
		char *getipbyname(char *domain);
		char *address_to_ip(char *address_in);
	protected:
		char localip[16];
		char gateway[16];
		char netmask[16];
		s32 netstate;
	
	private:
		int offset;
		u32 ipaddr;
		int red;
		char *buff;
		s32 socket;
		hostent *hostip;
		char *host;
};

class http : public internet{

public:
http();
~http();
char *gethttpfile(const char *file,char *host,int bufsize = 1025);
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

	

class email : public internet{
	

public:
	
	email();
	~email();
	void sendemail(struct emsg *mess);
	messlist *getallmail(); //must have set settings first; gets new mail + saved mail
	void setsettings(settype st,mailsettings *s);
	void getsettings(settype st,mailsettings *s);
	void clearsettings(settype st);
	messlist *getnewmail();
	int renderpopresponse(const char *resp);
	void parsemessage(messlist *ml,char *mess);
	void getsize(char *response,int *sizes,int numdata);
	

private:
	char *response;
	char *line;
	char *host;
	mailsettings *smtpsettings;
	mailsettings *popsettings;
	int popport;
	int smtpport;
	messlist *newmail;
	messlist *allmail;
	messlist *newmailroot;
	messlist *allmailroot;
	int mlen;
	char *mtmp;
	char *error;
	int nummessages;
	int rendered;
	int *messsize;
	char *mailbuffer;
	int numlines;
	char *messline;
	int pos;
	char *b;
	int bl;
	char *p;
	
	
};

typedef struct rssentry_s{
	char *title;
	char *description;
}rssentry;


class rss : public internet{
	public:
		rss();
		~rss();
		rssentry *getlatest();
		rssentry **getentries(int num);
	
	private:
};


class im : public internet{
	
};


class xmpp : public im{
	
};

class aim : public im{
	
};


class msn : public im{
	
};

class irc : public im{
	
};
