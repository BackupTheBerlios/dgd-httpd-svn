# include "./jorinde.h"

static void create(varargs int clone)
{
/*
	if(!find_object("sshd/initd")) {
		COMPILE("sshd/initd");
	}
*/
	if(!find_object(USR_SYSTEM+"httpd/initd")) {	
		COMPILE(USR_SYSTEM+"httpd/initd");
	}
}

