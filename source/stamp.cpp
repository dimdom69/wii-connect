/*
 *      stamp.cpp
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


#include "stamp.h"
#include <string.h>



stamp::stamp(char *f)
{
	file = f;
	ss = new stampsheet;
	pos = 0;
}


stamp::~stamp()
{
	
}

char *stamp::tonum(int n){
	tmp = new char [3+1];
	if(n<10){
		sprintf(tmp,"00%d",n);
	}
	else if(n<100){
		sprintf(tmp,"0%d",n);
	}
	else{
		sprintf(tmp,"%d",n);
	}
	return tmp;
}	
		
		
void stamp::loadheader()
{
	ss->header = new stamph;
	if((tmp=strstr(file,"Title"))){
		pos = tmp-file;
		pos += strlen("Title");
		tmp = strchr(file+pos, '=');
		pos = tmp-file;
		pos++;
		while((int)file[pos] < 33 && file[pos]) pos++;
		tmp = strchr(file+pos,'-');
		length = tmp-(file+pos);
		ss->header->title = new char [length+1];
		strncpy(ss->header->title,file+pos,length);
		ss->header->title[length] = '\0';
	}	
	if((tmp=strstr(file,"GameID"))){
		pos = tmp-file;
		pos += strlen("GameID");
		tmp = strchr(file+pos, '=');
		pos = tmp-file;
		pos++;
		while((int)file[pos] < 33 && file[pos]) pos++;
		tmp = strchr(file+pos,'-');
		length = tmp-(file+pos);
		ss->header->gameid = new char [length+1];
		strncpy(ss->header->gameid,file+pos,length);
		ss->header->gameid[length] = '\0';
	}	
	if((tmp=strstr(file,"Creator"))){
		pos = tmp-file;
		pos += strlen("Creator");
		tmp = strchr(file+pos, '=');
		pos = tmp-file;
		pos++;
		while((int)file[pos] < 33 && file[pos]) pos++;
		tmp = strchr(file+pos,'-');
		length = tmp-(file+pos);
		ss->header->creator = new char [length+1];
		strncpy(ss->header->creator,file+pos,length);
		ss->header->creator[length] = '\0';
	}
	if((tmp=strstr(file,"Num"))){
		pos = tmp-file;
		pos += strlen("Num");
		tmp = strchr(file+pos, '=');
		pos = tmp-file;
		pos++;
		while((int)file[pos] < 33 && file[pos]) pos++;
		tmp = strchr(file+pos,'-');
		length = tmp-(file+pos);
		tmp = new char [length+1];
		strncpy(tmp,file+pos,length);
		ss->header->num = atoi(tmp);
		delete [] tmp;
	}	
}

void stamp::loadfooter()
{
	
}
		
void stamp::loadstamps()
{
	for(int x = 0;x<ss->header->num;x++)
	{
		loadstamp(x);
	}
}

void stamp::loadstamp(int n)
{
	current = new stampa;
	current->num = n;
	tmp = strstr(file, tonum(n));
	if(!tmp) return;
	pos = tmp-file;
	pos += 3;
	tmp = strchr(file+pos,'=');
	pos = tmp-file;
	pos++;
	while((int)file[pos] < 33 && file[pos]) pos++;
	tmp = strchr(file+pos,',');
	length = tmp-(file+pos);
	current->title = new char [length+1];
	strncpy(current->title,file+pos,length);
	current->title[length] = '\0';
	pos = tmp-file;
	pos++;
	
	
	while((int)file[pos] < 33 && file[pos]) pos++;
	tmp = strchr(file+pos,'p');
	length = tmp-(file+pos);
	tmp = new char [length+1];
	strncpy(tmp,file+pos,length);
	tmp[length] = '\0';
	current->points = atoi(tmp);
	delete [] tmp;
	
	
	tmp = strchr(file+pos,',');
	pos++;
	while((int)file[pos] < 33 && file[pos]) pos++;
	tmp = strchr(file+pos,',');
	length = tmp-(file+pos);
	tmp = new char [length+1];
	strncpy(tmp,file+pos,length);
	tmp[length] = '\0';
	current->offset = (char*)atoi(tmp);
	delete [] tmp;
	
	
	
	tmp = strchr(file+pos,',');
	pos++;
	while((int)file[pos] < 33 && file[pos]) pos++;
	tmp = strchr(file+pos,',');
	length = tmp-(file+pos);
	tmp = new char [length+1];
	strncpy(tmp,file+pos,length);
	tmp[length] = '\0';
	current->value = (char*)atoi(tmp);
	delete [] tmp;
	
	ss->stamps.push_back(current);
}




bool stamp::checkstamp(int n){
	
}






void stamp::deletestamp()
{
	
}
