<%+
#include <kernel/kernel.h>
#include <type.h>

string argh() 
{
    object design;
	string ret;
	mixed **arr;
	int i;

	ret = "";
	i = 0;
    arr = session->get_breadcrumbs();
	for(i = sizeof(arr)-1; i > 0; i--) {
		ret += ((i<10)?"0"+i:i) + ". ";
		ret += ctime(arr[i][1]) + "&nbsp;&nbsp;";
		ret += "<a href=\"/" + arr[i][0] + "\">" + arr[i][0] + "</a>";
		ret += "<br>";
    }
	return ret;
}
+%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
    <meta http-equiv="content-type" content="text/html; charset=iso-8859-1" >
    <title>webserver/webdav in lpc (using DGD)</title>
    <link rel="stylesheet" type="text/css" href="../css/style.css" media="screen"/></head>
<body>
<div id="container">
    <div id="intro">
        <div id="preamble">
            <h3>
                <span>home</span>
            </h3>
            <p class="p1">
                <span>
                    Home of <acronym title="a web-server written in LPC">
                    Jorinde httpd</acronym>, written by Joakim Romland.
                </span>
            </p>
        </div>
    </div>

    <div id="supportingText">
        <div id="explanation">
            <h3><span>Breadcrumbs</span></h3>
            <p class="p1"><span>
<%= argh() %>
            </span></p>
	 </div>
    </div>
</div>
</body>
</html>
