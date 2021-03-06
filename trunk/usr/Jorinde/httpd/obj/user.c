/**
 * Description: User-object for Jorinde httpd.
 * Version:	 $Id$
 * License:	 (c)2004 Joakim Romland, see doc/License
 */
# include <kernel/kernel.h>
# include <kernel/user.h>
# include <kernel/access.h>
# include <status.h>
# include <limits.h>
# include <trace.h>
# include "../include/www.h"
# include "../include/access.h"

#define DEBUG_TRAFFIC_VERBOSE
#define DEBUG_INFO
#define DEBUG_WARNINGS
/*
#define DEBUG_TRAFFIC
#define DEBUG_TRAFFIC_VERBOSE
#define DEBUG_DATA
#define DEBUG_INFO
#define DEBUG_WARNINGS
*/

inherit user	LIB_HTTP_BUFF_USER;
inherit str		LIB_HTTP_STRING;
inherit 		LIB_DATE;
inherit cmds	LIB_HTTP_CMD_PLUGINS;

private	int			keep_alive;
private object		accessd, authend;
private	string		errors;
private Application	app;

#ifdef DEBUG_COST
private int		ticks_reqgen;			/* Cost for last request */
#endif

static Request	create_request(string str);


/**
 * Name: create()
 * Desc: Initialize this (cloned) user object.
 * Note: Kernel hook.
 * Args: Clone id
 * Rets: n/a
 */
static void create(int clone)
{
#ifdef __NO_CREATE_CLONE_ARG__
	if(!IS_CLONE(this_object())) { return; }
#else
	if(!clone) { return; }
#endif
	if(!sscanf(object_name(previous_object()), USR+"/System/%*s") 
			&& previous_object() != this_object()) {
		error("illegal call");
	}
	user::create();
	cmds::create();
	errors = nil;

	set_server( find_object(HTTP_SERVER) );
	authend = server->get_authentication_server();
	accessd = server->get_authorization_server();
}


/**
 * Name: set_application()
 * Desc: Set related application for this user.
 * Note: The application can change at any given time, objects should always
 *	   use get_application() to retrieve a user's application if they need
 *	   it.
 * Args: Application application
 * Rets: n/a
 */
void set_application(Application o)
{
	if(previous_program() != HTTP_USER && (Httpd)previous_object() != server) {
		error("illegal call");
	}
	app = o;
}


static int call_method(Request request, Response response)
{
	if(app->get_allowed_method( request->get_command() )) {
		return ::call_method(request, response);
	}
	return FALSE;
}


/**
 * Name: create_response()
 * Desc: Create a response from the request.
 * Note: n/a
 * Args: request object
 * Rets: A response (or nil and no response is sent).
 */
static Response create_response(Request request)
{
	Response response;
	string file, cmd;

	response = new_object(HTTP_RESPONSE);
	response->set_status(200);
	response->set_header("Date", datetime(time(), app->get_timezone()));
	response->set_header("Server", server->get_server_string());
	response->set_header("Keep-Alive", server->get_keep_alive_time());
	response->set_header("Cache-control", "private");
	/* TODO: Make expirytime configurable */
	response->set_header("Expires", 
							datetime(time(), app->get_timezone())
						);
	response->set_header("Content-Length", 0);
	response->set_header("Last-Modified", 
			datetime(time(), app->get_timezone()));
	response->set_request(request);

	if(app->get_session_tracking()) {
		response->set_cookie( app->get_session_cookie(request) );
	}

	if(request->get_badrequest()) {
		response->set_status(400);
		return response;
	}

	return response;
}


/**
 * Name: query_name()
 * Desc: Mask Kernel's query_name().
 * Note:
 * Args:
 * Rets:
 */
string query_name()
{
	if(::query_name()) {
		return ::query_name();
	}
	return HTTP_ANON_USER;
}


/**
 * Name: get_name()
 * Desc: Get name of this user.
 * Note: For consistency, we use get_* everywhere, kernel uses query_*
 * Args: n/a
 * Rets: Name of the user (or the anonymous user)
 */
string get_name()
{
	return query_name();
}


/**
 * Name: get_application()
 * Desc:
 * Note:
 * Args:
 * Rets:
 */
Application get_application()
{
	return app;
}


/**
 * Name: set_error()
 * Desc:
 * Note:
 * Args:
 * Rets:
 */
void set_error(string s)
{
	if(errors == nil) {
		errors = s;
	} else {
		errors += s;
	}
}

string get_error()
{
	string ret;
	ret = errors;
	errors = nil;
	return ret;
}

int has_error()
{
	return (errors != nil);
}


/**
 * Name: send_headers()
 * Desc:
 * Note: 
 * Args:
 * Rets:
 */
nomask int send_headers(Response response)
{
	string data;
	int status;

	/* Only user's tools should be able to call this */
	if(!is_tool(previous_object())) {
		error("illegal call");
	}

	status = response->get_status();
	if(response->get_content_length() == 0) {
		switch(status) {
		case 0..199 :
		/* 200-299 is no error */
		case 300..303 :
		/* 304 is no error (Not Modified) */
		case 305..999 :
			if(app->get_error_page(response) == FALSE) {
				LOG("Can't obtain status-page: " + status + "\n");
			}
		}
	}

	data  = response->status_tostring();
	data += response->headers_tostring();
	data += response->cookies_tostring();

#ifdef DEBUG_TRAFFIC_VERBOSE
	SYSLOG("-------- RESPONSE HEADERS --------\n");
	SYSLOG("\n" + data);
	SYSLOG("----------------------------------\n");
#endif

#ifdef DEBUG_TRAFFIC
	SYSLOG("res: " +
				"[" + ip_number() + "] " + 
				app->http_status_string(response->get_status()) + ", " +
				"len: " + response->get_content_length() + 
				"\n\n"
			);
#endif

	if(!send_data(data + CRLF)) {
#ifdef DEBUG_WARNINGS
		SYSLOG("Could not send headers. Destructing user object.");
#endif
		disconnect();
		return FALSE;
	}
	return TRUE;
}


nomask int send_content(Response response, string data)
{
	/* Only user's tools should be able to call this */
	if(!is_tool(previous_object())) {
		error("illegal call");
	}
	
	if(errors) {
		LOG("HTTP 503: " + errors);
		response->set_status(503);			/* or 200? */
		response->add_content(errors);
		response->update_content_length();
		errors = nil;
	}
#ifdef DEBUG_DATA
	SYSLOG("DATA: ---|\n" + data + "\n----|\n");
#endif
	return send_data(data);
}


/**
 * Masked event-function
 * Queried by the event-library, obj can be -any- type of object.
 */
nomask int can_subscribe(object obj, string name)
{
	return is_tool(obj);
}


/**
 * Name: receive_request()
 * Desc:
 * Note:
 * Args:
 * Rets:
 */
static int receive_request(string request_string)
{
	int ret;
	Request request;
	Response response;
	string hostname;

#ifdef DEBUG_COST
	rlimits(50; 16000000) { ticks_reqgen = AssessCost();
#endif

#ifdef DEBUG_TRAFFIC_VERBOSE
	SYSLOG("--------------- REQUEST ---------------\n");
	SYSLOG("From: " + ip_name() + " (" + ip_number() + ")\n" + request_string);
	SYSLOG("------------ END OF REQUEST -----------\n");
#endif

	ret = MODE_NOCHANGE;

	/*
	 * XXX TODO:
	 * This is a HACK; it can probably be exploited in a number of ways!
	 * But we DO want to know which application to use (as early as possible).
	 * Must figure out a way to fix so that we can set headers in the request
	 * object at this point. Or alternatives?
	 */
	if(sscanf(request_string, "%*sHost: %s\r\n%*s", hostname) == 3) {

		if(index_of(0, hostname, ":") != -1) {
			/* *sigh* Ugly fix. */
			hostname = explode(hostname, ":")[0];
		}

		set_application(server->get_application(hostname, get_port()));
	} else {
		set_application(server->get_application("", get_port()));
	}

	/* create a request */
	request = create_request(request_string);
	if(request == nil) {
#ifdef DEBUG_WARNINGS
		SYSLOG("request is nil (but stored in pending), incoming data\n");
#endif
		return MODE_NOCHANGE;
	}

#ifdef DEBUG_TRAFFIC
	SYSLOG("req: " + 
				"[" + ip_number() + "] " + 
				app->get_hostname() + ":" + 
				app->get_port() + ", " + 
				request->get_command() + " " +
				request->get_uri()->get_relative_path() + 
				request->get_uri()->get_filename() + ", " +
				"len: " + request->get_content_length() +
				"\n"
			);
#endif
	
	/* create a response */
	response = create_response(request);
	app->http_log(request, response);
	if(response == nil) {
		/* we got a content body with request, dealing with that */
#ifdef DEBUG_INFO
		SYSLOG("user->receive_request(), response is nil\n");
#endif
	} else {
		if(call_method(request, response) == FALSE) {
#ifdef DEBUG_WARNINGS
			SYSLOG("call_method failed; disconnecting\n");
#endif
			ret = MODE_DISCONNECT;
		}
	}

	if(!keep_alive && response->contents_buffered() == FALSE) {
#ifdef DEBUG_WARNINGS
		SYSLOG("receive_request(): no keep-alive nor buffered data. Bye.\n");
#endif
		ret = MODE_DISCONNECT;
	}

#ifdef DEBUG_COST
	ticks_reqgen = GetCost(ticks_reqgen); }
	OutputCost("Request cost", ticks_reqgen);
#endif

	return ret;
}


/*
 * TODO: This method should know NOTHING about what HTTP-methods exist,
 * it now does some magic if if the command is a POST. How can we get 
 * around this?
 */
static Request create_request(string str)
{
	Request request;
	string	cmd, ruri, ver, head, file, authstr, tmp;

	request = new_object(HTTP_REQUEST);

	if(!str||sscanf(str, "%s %s HTTP/%s\r\n%s", cmd, ruri, ver, head) !=4 ) {
		if(sscanf(str, "%s %s HTTP/%s\n%s", cmd, ruri, ver, head) != 4) {
			request->set_badrequest(TRUE);
#ifdef DEBUG_WARNINGS
			SYSLOG("create_request(): Bad request?\n");
#endif
			return request;
		}
	}

	/* Hack, we take for granted (twice) that version is 3 bytes */
	if(ver && strlen(ver) > 3) {
		string key, val;
		/*
		 * This is a BUG, and I -think- it is in the client. What happens
		 * is basically that \r\n is missing between HTTP-version and
		 * the first header after that (Connection for instance). This makes
		 * the request look like this:
		 * GET / HTTP/1.1Connection: Keep-Alive
		 *
		 * I know that Konqueror sometimes trigger this behaviour and throws
		 * an error saying "Connection is broken". This is due to the fact
		 * that we did not -see- the Connection header which is supposedly
		 * set to "Keep-Alive", so, our default behaviour is closing.
		 */
		SYSLOG("WARNING: version is a -long- string [" + ver + "]\n");
		head = ver[3..] + "\r\n" + head;
	}
	
	request->set_headers(head);
	request->set_command(cmd);
	request->set_protocol_version(ver);
	request->set_client_ip_name(ip_name());
	request->set_client_ip_number(ip_number());
	request->set_port(get_port());

	if(request->set_uri(ruri) == FALSE) {
		request->set_badrequest(TRUE);
		return request;
	}

	if(ver == "1.1" && request->get_header("Host") == nil) {
		request->set_badrequest(TRUE);
		return request;

	} else if(request->get_header("Host") == nil) {
		/* Cheating, but we always want a Host header. */
		request->set_header("Host", "");

	} else if(index_of(0, request->get_header("Host"), ":") != -1) {
		/* *sigh* Ugly fix. 
		 * TODO: This will not work under INET6
		 */
		request->set_header("Host", 
				explode(request->get_header("Host"), ":")[0]);
	}

	/* TODO: Must deal with more connection-types, this is a hack */
	if((tmp = request->get_header("Connection")) && 
			lower_case(tmp) == "keep-alive") {
		keep_alive = TRUE;
	} else {
		keep_alive = FALSE;
	}

	/* Authenticate user (if any) authenticate() returns user to log in */
	if(request->get_header("Authorization") && get_name() == HTTP_ANON_USER &&
				(authstr = authend->authenticate(request)) && strlen(str)) {
#ifdef DEBUG_INFO
		SYSLOG("logging in '" + authstr + "'...\n");
#endif
		user::login( authstr, 1 );
#ifdef DEBUG_INFO
		SYSLOG("logged in name is: " + query_name() + "\n");
#endif
		request->set_header("Authorization", "[secret]");
		request->set_authenticated( get_name() ? get_name() : "" );
		clone_tools();
		
	} else if(commandcount() == 0) {
		clone_tools();
	}

	/* POST requests don't need to set content-length in headers */
	if((request->get_header("Content-Length") 
			&& request->get_header("Content-Length") != "0") 
			|| cmd == "POST") {
		int len;
		string content;
		Content ob;
	
		if(request->get_header("Content-Length")) {
			len = (int)request->get_header("Content-Length");
		} else {
			len = 0;
		}

#ifdef DEBUG_DATA
		SYSLOG("Claimed tot: " + len + " bytes (got: "+strlen(str)+")\n");
#endif

		if(sscanf(str, "%*s" + CRLF + CRLF + "%s", content) == 2) {
#ifdef DEBUG_DATA
			SYSLOG("adding some content\n");
#endif
		} else if(cmd == "POST") {
#ifdef DEBUG_DATA
			SYSLOG("expecting content\n");
#endif
		} else {
			/* Headers did not end with 2*CRLF, bad request */
#ifdef DEBUG_WARNINGS
			SYSLOG("malformed request\n");
#endif
			request->set_badrequest(TRUE);
			return request;
		}

#ifdef DEBUG_DATA
		SYSLOG("expecting more content\n");
#endif
		ob = set_incoming();
		ob->set_request(request);

		if(content != nil && strlen(content)) {
			request->add_content(content);
			receive_pending( request->content_tostring() );
		} else {
#ifdef DEBUG_DATA
			SYSLOG("no content with request\n");
#endif
		}

		return nil;
	}

	return request;
}


int receive_message(string str)
{
	string initial_req;

	/*
	 * SECURITY:
	 * Is there a way to have previous_program() set to nil? If yes, this
	 * is a terrible security hole.
	 */
	if(previous_program() && previous_program() != LIB_CONN) {
		error("illegal call");
	}

	initial_req = get_first_line();
	initial_req = (initial_req ? initial_req : "");
	if(pending_content() || sscanf(initial_req + str, "%*s\r\n\r\n") != 1) {
		return ::receive_message(str);
	}

	set_first_line("");
	return receive_request((initial_req=="" ? "" : initial_req+"\r\n") + str);
}
