/*
 *      http.cpp
 * 
 * 		This contains the http class, the class that handles all network functions
 * 		and is the parent of all derivitives of the internet (eg email, IM, etc.)
 *      
 *      Copyright 2009 Aaron Lehrian (JSMaster) <jsmaster@tampabay.rr.com>
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




#include <errno.h>
#include <fat.h>
#include <math.h>
#include <debug.h>
#include <network.h>
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/dir.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "internet.h"



/***********************************************************************
 * http(): Simply sets up the networking on the Wii so that you can
 * connect to the internet and such
 * 
 * ********************************************************************/
http::http(){
}

http::~http(){
	
}


char *http::gethttpfile(const char *file,char *host,int bufsize,int snum){
	netstate = 5;
	if(host == NULL){	//Sometimes works, not always (will use http 1.0 instead of 1.1. Does not work for google, eg)
		httpget = new char [strlen("GET / HTTP/1.0\r\n\r\n")+strlen(file)+1];  
		if(httpget == NULL){  //Memory checking
			netstate = -10;
			return NULL;
		}
		asprintf(&httpget,"GET /%s HTTP/1.0\r\n\r\n",file); //Simplest way to GET a file
		writetosocket(httpget,snum);       //Send the request
		delete [] httpget;		//Save memory
		netstate = 0;
		return readfromsocket(bufsize,snum);	//Recieve the response
	}
	else{				//Prefered Method
		httpget = new char [strlen("GET / HTTP/1.1\r\nHost: \r\n\r\n")+strlen(file)+strlen(host)+1];
		if(httpget == NULL){ //Memory...
			netstate = -10;
			return NULL;
		}
		asprintf(&httpget,"GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n",file,host); //Better way to GET a file, should always work if used correctly
		writetosocket(httpget,snum); //Send it
		delete [] httpget;  //Save memory
		netstate = 0;
		return readfromsocket(bufsize,snum);  //Recieve response (hopefully the file you wanted) and return it
	}
}

