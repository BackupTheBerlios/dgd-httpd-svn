<%+
string test()
{
	string ret;
	ret = "";
	ret += "     server object: " +	object_name(server) + "<br>";
	ret += "   response object: " +	object_name(response) + "<br>";
	ret += "    request object: " +	object_name(request) + "<br>";
	ret += "application object: " +	object_name(application) + "<br>";
	ret += "    session object: " +	object_name(session) + "<br>";
    return ret;
}

+%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Hands of Joakim Romland">
   <title>wrapper test</title>
</head>
<body>

<pre><%=test()%></pre>

</body>
</html>

