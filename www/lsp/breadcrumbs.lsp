<%+
#include <kernel/kernel.h>
#include <type.h>
#if 0
#define OB "/usr/www/design/01"
#endif

void argh() 
{
    object design;
	string ret;
	mixed **arr;
	int i;
#if 0
	if(!find_object(OB))
		compile_object(OB);

	design = find_object(OB);
	design->init();
#endif
	design->title("Breadcrumbs");
	design->left_para("LSP",
		"You're currently looking at an LPC Server Page, it's a simple " +
		"implementation so far. The parser sucks and we don't do " +
		"recompiling-on-demand of related objects (inherited or included).");

	ret = "";
	i = 0;
    arr = session->get_breadcrumbs();
	for(i = sizeof(arr)-1; i > 0; i--) {
		ret += ((i<10)?"0"+i:i) + ". ";
		ret += ctime(arr[i][1]) + "&nbsp;&nbsp;";
		ret += "<a href=\"/" + arr[i][0] + "\">" + arr[i][0] + "</a>";
		ret += "<br>";
    }
	design->main_para("Your breadcrumbs in this session", ret);
	response->write( design->content() );
}
+%>
<%
	argh();
%>

