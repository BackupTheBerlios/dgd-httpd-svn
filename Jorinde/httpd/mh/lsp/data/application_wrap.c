/**
 * Description: Wrapper class for application that's used by LSP's.
 * Version:     $Id: application_wrap.c 167 2004-06-01 08:42:16Z romland $
 * License:     (c)2004 Joakim Romland, see doc/License
 */
# include <kernel/kernel.h>
# include "../../../include/www.h"
# include <type.h>

inherit LIB_HTTP_STRING;
#ifdef __IGOR__
inherit "/lib/lwo";
#endif

static object request, response, application, server, session;

void set_related(object req, object res, object app, object ser, object ses) {
	request = req;
	response = res;
	application = app;
	server = ser;
	session = ses;
}

void create(varargs int clone)
{
	/* ... */
}

/* TODO: This file. */
string	get_webroot()				{ return application->xxx(); }
string	get_lsp_home()				{ return application->xxx(); }
string	get_timezone()				{ return application->xxx(); }
string	get_hostname()				{ return application->xxx(); }
string	get_status_string(int a)	{ return application->xxx(); }

/* TODO: Add possibility to add application-content */

