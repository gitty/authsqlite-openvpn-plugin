/*------------------------------------------------------------------------------
authsqlite Copyright (c) 2007, authentication plugin for OpenVPN
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
#include <string.h>
#include <stdlib.h>

#include "openvpn-plugin.h"
#include "sqlite3.h"
#include "sha256.h"

#define PARAM_MAX_SIZE 40 /* maximal size of username and password */

/* our context, where we keep our state */
struct plugin_context
{
	sqlite3 *db;
	int lastrows;
};

static int callback(void *handle, int argc, char **argv, char **azColName)
{
	struct plugin_context *context = (struct plugin_context *) handle;

	/* increase number of records found for our sql statement */
	context->lastrows++;
	return 0; /* proceed execution */
}

int executeSQL( const char* sql, openvpn_plugin_handle_t handle )
{
	struct plugin_context *context = (struct plugin_context *) handle;
	int rc;
	char *zErrMsg = 0;

	if ( context->db == NULL )
	{
		fprintf(stderr, "AUTHSQLITE: Executing SQL failed. Database not open!\n");
		return 1; /* error */
	}

	context->lastrows = 0; /* reset found rows */

	/* execute sql statement */
	rc = sqlite3_exec(context->db, sql, callback, (void*)handle, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "AUTHSQLITE: SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 1; /* error */
	}

	return 0; /* succeed */
}

/* Given an environmental variable name, search
* the envp array for its value, returning it
* if found or NULL otherwise. */
static const char *
get_env (const char *name, const char *envp[])
{
	if (envp)
	{
		int i;
		const int namelen = strlen (name);
		for (i = 0; envp[i]; ++i)
		{
			if (!strncmp (envp[i], name, namelen))
			{
				const char *cp = envp[i] + namelen;
				if (*cp == '=')
					return cp + 1;
				}
			}
	}
	return NULL;
}

OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1 (unsigned int *type_mask, const char *argv[], const char *envp[])
{
	struct plugin_context *context;
	int rc;

	/* allocate our context */
	context = (struct plugin_context *) calloc (1, sizeof (struct plugin_context));

	/* try to open database and save in context */
	rc = sqlite3_open( argv[1], &(context->db));
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "AUTHSQLITE: Can't open database \"%s\": %s\n", argv[1], sqlite3_errmsg(context->db));
		sqlite3_close(context->db);
		return NULL; /* error, we can't serve anything */
	}

	*type_mask = OPENVPN_PLUGIN_MASK (OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);
	return (openvpn_plugin_handle_t) context;
}

OPENVPN_EXPORT int
openvpn_plugin_func_v1 (openvpn_plugin_handle_t handle, const int type, const char *argv[], const char *envp[])
{
	struct plugin_context *context = (struct plugin_context *) handle;

	char* username = calloc(1, PARAM_MAX_SIZE+1);
	char* password = calloc(1, PARAM_MAX_SIZE+1);
	char* digpwd = calloc(1, 65);
	char* sql = calloc(1, 255);
	int ret;

	/* get username/password from envp string array */
	memcpy( username, get_env("username", envp), PARAM_MAX_SIZE );
	memcpy( password, get_env("password", envp), PARAM_MAX_SIZE );

	if ( username == NULL || password == NULL || strlen(username) < 1 || strlen(password) < 1 )
		return OPENVPN_PLUGIN_FUNC_ERROR;

	genSHA256(password, strlen(password), digpwd);

	/* form our sql statement to lookup database */
	sprintf( sql, "SELECT username FROM users WHERE username = '%s' AND password = '%s' AND enabled = 1", username, digpwd );
	free( username );
	free( password );
	free( digpwd );

	ret = executeSQL( sql, handle );
	free( sql );

	/* executeSQL must return 0 and
	there must be exactly 1 answer record for positive authentication */
	if ( ret || context->lastrows != 1 )
		return OPENVPN_PLUGIN_FUNC_ERROR;
	else
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

OPENVPN_EXPORT void
openvpn_plugin_close_v1 (openvpn_plugin_handle_t handle)
{
	struct plugin_context *context = (struct plugin_context *) handle;

	/* close database and free context */
	if ( context->db != NULL )
		sqlite3_close(context->db);
	free( context );
}

