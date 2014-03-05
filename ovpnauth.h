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
#ifndef _OVPNAUTH_H_
#define _OVPNAUTH_H_

#define PROG_VERSION "1.1.1"
#define PARAM_MAX_SIZE 40

#define MODE_ADD 1
#define MODE_REMOVE 2
#define MODE_CHANGE 3
#define MODE_ENABLE 4
#define MODE_DISABLE 5
#define MODE_SHOW 6

enum change_type {
     DB_UPDATE = 0,
     DB_INSERT = 1,
     DB_DELETE = 2
     };

static const char usage_message[] =
  "Usage: %s <options> file\n"
  "\n"
  "General Options:\n"
  "-h          : Show usage and version.\n"
  "\n"
  "Database Operations:\n"
  "-a -u <user> -p <pass>    : Add a new user \"user\" with password \"pass\".\n"
  "-c -u <user> -p <newpass> : Change password to \"newpass\" for user \"user\".\n"
  "-r -u <user>     : Remove existing user \"user\".\n"
  "-s [-u <user>]   : Show user \"user\" or all users.\n"
  "-e -u <user>     : Allow user \"user\" to log in.\n"
  "-d -u <user>     : Deny user \"user\" to log in.\n"
  "----------------------------------------------------\n"
  "Examples: ovpnauth -a -u user1 -p password1 db1.db\n"
  "          ovpnauth -c -u user1 -p password2 db1.db\n"
  "          ovpnauth -d -u user1 db1.db\n"
  "          ovpnauth -r -u user1 db1.db\n"
  ;

#endif
