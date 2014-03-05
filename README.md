# OpenVPN authentication plugin with SQLite backend #

This plugin for OpenVPN use a sqlite database backend to check users who log in using auth-user-pass.

Windows users: If you want to use the binary package (not compiling yourself), you only need a running OpenVPN installation, all other files are provided by the package. Proceed straight to the "Installation and Configuration" section below.

You should be aware that it is a bad idea to use precompiled binaries from unknown sources for security related applications.

## Prerequisites ##

* OpenVPN installed and running, see http://openvpn.net for more informations and downloads
* SQLite3 libraries and header files installed, see http://www.sqlite.org for more informations and downloads
* GCC (Linux) or MinGW (Windows) if you want to compile yourself, see http://www.mingw.org

## Compiling for Linux ##

* Find out where *sqlite3.h* (maybe in */usr/include* or */usr/local/include*) and where libsqlite.so is (*/usr/lib* or */usr/local/lib*).
* Modify *Makefile* in top directory to fit these paths.
* Run *make* in top directory.

## Compiling for Windows ##

To compile natively on Windows, use either DevCpp or MinGW.

**DevCpp:** If you use DevCpp (see http://www.bloodshed.net/devcpp.html for downloads), you can use the project file authsqlite.dev in the win32/ subdirectory of the source. You should be able to build a DLL from the menu.

**MinGW:** If you use MinGW, modify *Makefile.win* in the *win32/* subfolder to fit your paths. Then run *make* in *win32/* subdirectory.

## Preparing the database ##

You **must** create a table in a database file and add username/password pairs to this table:

Start the *ovpnauth* tool with the *-s* option and your database file. The program will create the proper table in a database file called *database-file.db* (see *ovpnauth -h* for more help): *ovpnauth -s database-file.db*

Alternatively, create the file yourself. You may run the *sqlite3* application which is part of the SQLite3 package. Execute the following line to create a database called *database-file.db*: *sqlite3 database-file.db*
For Windows, download a precompiled version of sqlite3 from their homepage (see link above).
Then type in the following SQL statement (without the lines) and press enter:

    CREATE TABLE users ( username[char(40)][NOT NULL], password[char(70)][NOT NULL], enabled[int(1)][NOT NULL], PRIMARY KEY(username), UNIQUE(username) );

To exit sqlite3 application, type *.quit*

**Caution:** Please be aware that only OpenVPN (and root) should have access to the database file.

## Installation and Configuration ##

**Linux:** Copy *authsqlite.so* and your database file to your OpenVPN configuration directory or wherever you prefer. Modify your OpenVPN server configuration and add this line to your server configuration:

    plugin /path/to/authsqlite.so /path/to/database-file.db

**Windows:** Copy *authsqlite.dll*, *sqlite3.dll* and your database file to your OpenVPN configuration directory or wherever you prefer. Then modify your OpenVPN server configuration and add this line to your server configuration:

    plugin C:\path\to\authsqlite.dll C:\path\to\database-file.db

Restart the OpenVPN server process. When you run OpenVPN with your new configuration it should report a positive initialization of the plugin. If not, you may need to tune the *verb* and *mute* confguration file settings to get more output.
