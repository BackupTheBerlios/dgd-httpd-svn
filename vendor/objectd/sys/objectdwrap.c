#include "~/jorinde.h"

#define Jorinde() (sscanf(object_name(previous_object()),USR+"/Jorinde/%*s")==1)

string report_on_object(string spec)
{
	if(!Jorinde()) error("illegal call");
	return (OBJECTD)->report_on_object(spec);
}

mixed* od_status(mixed obj_spec)
{
	if(!Jorinde()) error("illegal call");
	return (OBJECTD)->od_status(obj_spec);
}

string destroyed_obj_list(void)
{
	if(!Jorinde()) error("illegal call");
	return (OBJECTD)->destroyed_obj_list();
}

void recompile_auto_object(object output)
{
	if(!Jorinde()) error("illegal call");
	(OBJECTD)->recompile_auto_object(output);
}
