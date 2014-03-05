/*------------------------------------------------------------------------------
ovpnauth Copyright (c) 2007, frontend for database operations
Copyright (C) 2007  noteo [at] trooth [dot] cc

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
----------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ovpnauth.h"

static char* prgname;

void openDB( char *dbname ) {
	 int ret;
     openDatabase(dbname);
     executeSQL( "CREATE TABLE IF NOT EXISTS users \
                 (username[char(40)][NOT NULL], password[char(80)][NOT NULL], \
                 enabled[int(1)][NOT NULL], PRIMARY KEY(username), \
                 UNIQUE(username))", 1 );
}

void print_sql_header()
{
	printf( "Username | Enabled |\n" );
	printf( "--------------------\n" );
}

void usage() {
     fprintf(stdout, "%s version: %s\n", prgname, PROG_VERSION);
     fprintf(stdout, usage_message, prgname);
     exit(1);
}

void show_user( char* dbname, const char* user )
{
    #ifdef ODEBUG
           fprintf(stderr, "Showing user %s for db: %s\n", user, dbname);
    #endif
	char* newSQL = malloc(255);
	if (user != NULL)
	   sprintf( newSQL, "SELECT username, enabled FROM users WHERE username LIKE '%s'", user );
    else
       sprintf( newSQL, "SELECT username, enabled FROM users ORDER BY username ASC", user );

	openDB( dbname );
	print_sql_header();
	executeSQL( newSQL, 1 );
	showAffected();
	closeDatabase();
	
	free(newSQL);
	return;
}

void add_user( char *dbname, const char *user, const char *password )
{
    #ifdef ODEBUG
           fprintf(stderr, "Adding user %s, password %s for db: %s\n", user, password, dbname);
    #endif
	char* newSQL = malloc(sizeof(char)*255);
	char *digpwd = malloc(65);
	
	genSHA256((char*)password, strlen(password), digpwd);
	
	sprintf( newSQL, "INSERT INTO users (username, password, enabled) VALUES ('%s', '%s', 1)", user, digpwd );
	
	openDB( dbname );
	executeSQL( newSQL, 1 );
	showChanged( DB_INSERT );
	closeDatabase();
	
	free(newSQL);
	free(digpwd);
	return;
}

void delete_user( char* dbname, const char* user )
{
    #ifdef ODEBUG
           fprintf(stderr, "Removing user %s for db: %s\n", user, dbname);
    #endif
	char* newSQL = malloc(255);
	sprintf( newSQL, "DELETE FROM users WHERE username = '%s'", user );
	
	openDB( dbname );
	executeSQL( newSQL, 1 );
	showChanged( DB_DELETE );
	closeDatabase();
	
	free(newSQL);
	return;
}

void change_access_user( char* dbname, const char* user, int enableDisable )
{
    #ifdef ODEBUG
           fprintf(stderr, "Changing enable/disable user %s for db: %s, flag: %d\n", user, dbname, enableDisable);
    #endif
	char* newSQL = malloc(255);
	sprintf( newSQL, "UPDATE users SET enabled = %d WHERE username = '%s'", enableDisable, user );

	openDB( dbname );
	executeSQL( newSQL, 1 );
	showChanged( DB_UPDATE );
	closeDatabase();
	
	free(newSQL);
	return;
}

void change_pass_user( char* dbname, const char* user, const char* password )
{
    #ifdef ODEBUG
           fprintf(stderr, "Changing user %s password %s for db: %s\n", user, password, dbname);
    #endif
	char* newSQL = malloc(255);
	char *digpwd = malloc(65);
	
	genSHA256((char*)password, strlen(password), digpwd);
	sprintf( newSQL, "UPDATE users SET password = '%s' WHERE username = '%s'", digpwd, user );
	
	openDB( dbname );
	executeSQL( newSQL, 1 );
	showChanged( DB_UPDATE );
	closeDatabase();
	
	free(newSQL);
	return;
}

int main(int argc, char **argv)
{ 
    char c;
    char *username, *password, *dbname;
    int uc, pc;
    int mode;
    prgname = argv[0];
    uc = 0;
    pc = 0;
    mode = 0;
    username = NULL;
    password = NULL;
    
    while((c = getopt(argc, argv, "acdehu:p:rs")) != -1) {
        switch(c) {
        case 'a':
           if (mode == 0) mode = MODE_ADD; else usage();
           break;
        case 'c':
           if (mode == 0) mode = MODE_CHANGE; else usage();
           break;
        case 'd':
           if (mode == 0) mode = MODE_DISABLE; else usage();
           break;
        case 'e':
           if (mode == 0) mode = MODE_ENABLE; else usage();
           break;
        case 'r':
           if (mode == 0) mode = MODE_REMOVE; else usage();
           break;
        case 's':
           if (mode == 0) mode = MODE_SHOW; else usage();
           break;
        case 'u':
           username = optarg;
           uc++;
           break;
        case 'p':
           password = optarg;
           pc++;
           break; 
        case 'h':
        default:
           usage();
           break;
        }
    }
    
    dbname = argv[optind];
    
    if (dbname == NULL)
       usage();
    
    #ifdef ODEBUG
           fprintf(stderr, "DBname: %s\n", dbname);
    #endif
    
    if ((uc > 1) || (pc > 1)) {
       fprintf(stderr, "Too many username/password options specified.\n");
       usage();
       exit(1);
    }
    
    if (mode == 0) {
       fprintf(stderr, "No operation mode specified.\n");
       usage();
       exit(1);
    }

    if (username != NULL && (strlen(username) > PARAM_MAX_SIZE)) {
       fprintf(stderr, "Username too long\n");
       exit(1);
    }
  
    if (password != NULL && (strlen(password) > PARAM_MAX_SIZE)) {
       fprintf(stderr, "Password too long\n");
       exit(1);
    }
    
    switch(mode) {
    case MODE_SHOW:
         if ((uc >= 0) && (pc == 0))
            show_user(dbname, username);
         else
             usage();
         break;
    case MODE_ADD:
         if ((uc == 1) && (pc == 1))
          add_user( dbname, username, password );
         else
          usage();
         break;
    case MODE_CHANGE:
         if ((uc == 1) && (pc == 1))
          change_pass_user( dbname, username, password );
         else
          usage();
         break;
     case MODE_REMOVE:
         if ((uc == 1) && (pc == 0))
          delete_user( dbname, username );
         else
          usage();
         break;   
     case MODE_ENABLE:
         if ((uc == 1) && (pc == 0))
          change_access_user( dbname, username, 1 );
         else
          usage();
         break;
     case MODE_DISABLE:
         if ((uc == 1) && (pc == 0))
          change_access_user( dbname, username, 0 );
         else
          usage();
         break;
     default:
          usage();          
    }
  
  exit(0);
}
