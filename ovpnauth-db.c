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
#include "ovpnauth.h"
#include "sqlite3.h"
#include "sha256.h"

static sqlite3 *db;
static int lastrows = 0;

int openDatabase( char* dbname )
{
  int rc;
  
  if ( db != NULL )
  	return 0;

  rc = sqlite3_open(dbname, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }
  return rc;
}

void closeDatabase()
{
	if ( db != NULL )
		sqlite3_close(db);
}

int rowsAffected()
{
	return lastrows;
}

void showAffected()
{
	printf( "Found %d entries in database.\n", lastrows );
}

void showChanged( int change )
{
	if ( db != NULL )
	{	
		printf( "%d entries ", sqlite3_changes( db ) );
			
		switch( change )
		{
			case DB_UPDATE:
				printf( "updated.\n" );
				break;
			case DB_INSERT:
				printf( "inserted.\n" );
				break;
			case DB_DELETE:
				printf( "deleted.\n" );
				break;
		}
	}
}

static int callback(void *showOutput, int argc, char **argv, char **azColName)
{
	int i;
	if ( (int)showOutput == 1 )
	{
		for( i=0; i<argc; i++ )
		{
			printf("%s | ", argv[i] ? argv[i] : "NULL");
		}
		
		printf("\n");
	}

	lastrows++;
	return 0;
}

void executeSQL( const char* sql, int showOutput )
{
	int rc;
	char *zErrMsg = 0;
	
	if ( db == NULL )
		exit(1);
	
	lastrows = 0;
	rc = sqlite3_exec(db, sql, callback, (void*)showOutput, &zErrMsg);
	
	if( rc != SQLITE_OK )
	{
	  fprintf(stderr, "SQL error: %s\n", zErrMsg);
	  free(zErrMsg);
	  closeDatabase(db);
	  exit(1);
	}
}
