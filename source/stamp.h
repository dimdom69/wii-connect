/*
 *      stamp.h
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


#ifndef STAMP_H
#define STAMP_H

#include <vector>

using namespace std;

typedef struct stampc_s{
	char *console;
	char *subconsole;
	char *genre;
	char *rating;
	char *developer;
	char *publisher;
}stampc;


typedef struct stamph_s{
	char *title;
	char *gameid;
	char *creator;
	int num;
}stamph;

typedef struct stampf_s{
	char *info;
	stampc *category;
}stampf;

typedef struct stampa_s{
	int num;
	char *title;
	int points;
	char *offset;
	char *value;
}stampa;


typedef struct stampsheet_s{
	stamph *header;
	vector<stampa*> stamps;
	stampf *footer;
}stampsheet;

class stamp
{
	public:
		//dont worry about first, it is only used internally by the class
		stamp(char *f);
		virtual ~stamp();
		void loadstamps();
		void loadstamp(char *snum);
		void deletestamp();
		char *tonum(int n);
		void loadheader();
		void loadfooter();
		
	
	private:
		stampsheet *ss;
		int length;
		int pos;
		char *tmp;
		char *file;
		stampa *current;
};

#endif /* STAMP_H */ 
