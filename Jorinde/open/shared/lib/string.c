/**
 * Description: General purpose string library.
 * Version:     $Id: string.c 168 2004-06-01 19:31:01Z romland $
 * License:     (c)2004 Joakim Romland, see doc/License
 */

# include "../include/shared.h"
# include <kernel/kernel.h>
# include <type.h>

inherit "./array";

static string trim(string s);

/**
 * @desc:		Find first occurance of 'look_for' in 'look_in' from index
 * 				'look_from' and on.
 * @arguments:	Start position, string to look in, string to look for.
 * @returns:	-1 on not found.
 *				0 or more on found; first character is 0.
 */
static int index_of(int look_from, string look_in, string look_for)
{
	string garbage;
	int len_li;

	len_li = strlen(look_in)-1;
	if(look_from >= len_li){
		return -1;
	}

	if(sscanf(look_in[look_from..len_li],"%s"+look_for+"%*s",garbage)==0) {
		return -1;
	} else {
		return strlen(garbage) + look_from;
	}
}

static string lower_case(string str)
{
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += 'a' - 'A';
		}
	}
	return str;
}

static string upper_case(string str)
{
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] >= 'a' && str[i] <= 'z') {
			str[i] -= 'a' - 'A';
		}
	}
	return str;
}

static string capitalize(string str)
{
	if(str[0] >= 'a' && str[0] <= 'z') {
		str[0] -= 'a' - 'A';
	}
	return str;
}

static string indent(int in, string str)
{
	string ret;
	ret = "";
	while(in >= 0 && in--) {
		ret += str;
	}
	return ret;
}

#if 0
/**
 * Accepts DGD's date format in string or integer.
 * Netscape:	Wdy, DD-Mon-YYYY HH:MM:SS GMT
 *				Sun, 17-Jan-2038 19:14:07 GMT
 *				01234567890123456789012345678
 * DGD:			Wed Jan 21 20:10:25 2004
 *				012345678901234567890123
 */
/* TODO: RFC 2068, date format */
static string datetime(mixed arg, varargs string zone)
{
	string x;
	if(typeof(arg) == T_STRING && strlen(arg) != 24) {
		return arg;
	}

	x = ( (typeof(arg)==T_INT) ? ctime(arg) : arg );
	x = x[0..2] + ", " + x[8..9] + "-" + x[4..6] + "-" + x[20..23] + " "
	  + x[11..18] + "" + (zone ? zone : "");
	return x;
}

/* TODO: -Almost- duplicate code, see if we can't re-use the function above 
<a:getlastmodified b:dt="dateTime.rfc1123">
    Wed, 17 Mar 2004 18:15:49 GMT
</a:getlastmodified>
*/
static string datetime_rfc1123(mixed arg, varargs string zone)
{
	string x;
	if(typeof(arg) == T_STRING && strlen(arg) != 24) {
		return arg;
	}

	x = ( (typeof(arg)==T_INT) ? ctime(arg) : arg );
	x = x[0..2] + ", " + x[8..9] + " " + x[4..6] + " " + x[20..23] + " "
	+ x[11..18] + "" + (zone ? zone : "");
	return x;
}


# define SHORTMONTHS	({ "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" })
# define SHORTDAYS		({ "sun", "mon", "tue", "wed", "thu", "fri", "sat" })
# define LONGDAYS		({ "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" })

# define DAYSPERMONTH	({ 31,28,31,30,31,30,31,31,30,31,30,31 })

# define IsLeapYear(x)	( !((x) % 4) && ( ((x) % 100) || !((x) % 400) ) )

# define DT_WEEKDAY		0
# define DT_DATE		1
# define DT_MONTH		2
# define DT_YEAR		3
# define DT_HOUR		4
# define DT_MINUTE		5
# define DT_SECOND		6
# define DT_ZONE		7
# define DT_SIZE		8
# define NewDT()		allocate(DT_SIZE)

static int get_dayofyear(int year, int month, int date)
{
	int ret;

	ret = 0;
	if(IsLeapYear(year) && month > 1) {
		ret += 1;
	}

	while(month--) {
		ret += DAYSPERMONTH[month];
	}

	ret += date;
	return ret;
}


static int dt_to_int(mixed *dt)
{
	string strval;
	int *intdt;
	int ret, yearday, i;

	intdt = NewDT();
	intdt[DT_YEAR]   = (int)dt[DT_YEAR];
	intdt[DT_DATE]   = ((int)dt[DT_DATE]) - 1;
	intdt[DT_MONTH]  = (int)index_of_arr(dt[DT_MONTH], SHORTMONTHS);
	intdt[DT_HOUR]   = ((int)dt[DT_HOUR]) - 1;
	intdt[DT_MINUTE] = (int)dt[DT_MINUTE];
	intdt[DT_SECOND] = (int)dt[DT_SECOND];

	yearday = get_dayofyear(intdt[DT_YEAR], intdt[DT_MONTH], intdt[DT_DATE]);
	ret = intdt[DT_SECOND] + 60 * 
		  (intdt[DT_MINUTE] + 60 * (intdt[DT_HOUR] + 24 * yearday));

	i = 1970;
	while(i < intdt[DT_YEAR]) {
		if(IsLeapYear(i)) {
			ret += 31622400;
		} else {
			ret += 31536000;
		}
		i++;
	}

	return ret;
}


static int datetime_rfc850_to_int(string val)
{
	error("TODO: datetime_rfc850_to_int()");
}


/* Wed, 17 Mar 2004 18:15:49 GMT */
static int datetime_rfc1123_to_int(string val)
{
	mixed *dt;
	dt = NewDT();
	
	val = lower_case(val);
	dt[DT_WEEKDAY] = val[0..2];
	dt[DT_DATE]    = trim(val[5..6]);
	dt[DT_MONTH]   = val[8..10];
	dt[DT_YEAR]    = val[12..15];
	dt[DT_HOUR]    = val[17..18];
	dt[DT_MINUTE]  = val[20..21];
	dt[DT_SECOND]  = val[23..24];
	dt[DT_ZONE]    = val[26..27];
	return dt_to_int(dt);
}


/* Wed Jan 21 20:10:25 2004 */
static int ctime_to_int(string val)
{
	mixed *dt, *time;
	dt = NewDT();

	val = lower_case(val);
	dt[DT_WEEKDAY] = val[0..2];
	dt[DT_MONTH]   = val[4..6];
	dt[DT_DATE]    = val[8..9];
	time = explode(val[11..18], ":");
	dt[DT_HOUR]    = time[0];
	dt[DT_MINUTE]  = time[1];
	dt[DT_SECOND]  = time[2];
	dt[DT_YEAR]    = val[20..23];

	return dt_to_int(dt);
}


static int datetime_to_int(string val)
{
	if(strlen(val) < 4) {
		error("invalid date");
	}

	if(val[3] == ',') {
		/* RFC 822, updated by RFC 1123 */
		return datetime_rfc1123_to_int(val);

	} else if(val[3] == ' ' && strlen(val) != 24) {
		/* RFC 850, obsoleted by RFC 1036 */
		return datetime_rfc850_to_int(val);

	} else {
		/* ANSI C's asctime() format */
		return ctime_to_int(val);
	}
}
#endif


/** 
 * Slower than the one in the kernel wiztool but I had this lying around
 * and it will do the trick.
 */
static string make_string(mixed m)
{
	if(typeof(m) == T_INT || typeof(m) == T_FLOAT)
		return (string)m;

	if(typeof(m) == T_NIL) {
		return "nil";
	}

	if(typeof(m) == T_STRING) {
		m = implode(explode("\n" + m + "\n", "\n"), "\\n");
		m = implode(explode("\"" + m + "\"", "\""), "\\\"");
		return "\"" + m + "\"";
	}

	if(typeof(m) == T_OBJECT) {
		if(m->is_dom_element()) {
			return "(obj)" + m->getTypeName() + ":" + m->getName();
		}
		return "(obj)" + object_name(m);
	}

	if(typeof(m) == T_ARRAY) {
		return "({ " + implode(
							map_array( m, "make_string", this_object() ),
				", ") + ", })";
	}

	if(typeof(m) == T_MAPPING) {
		int	i;
		string str;
		mixed  *ind, *val;

		ind = map_indices(m);
		val = map_values(m);
		str = "([ ";
		for(i = 0; i < sizeof(ind); i++)
			str += make_string(ind[i]) + ": " + make_string(val[i])+", ";
		return str + "])";
	}

	return "unknown type, " + typeof(m) + ".";
}

/* TODO: Perhaps support more datatypes? */
static int is_numeric(string val)
{
	int i;
	i = strlen(val);

	if(!i) {
		return FALSE;
	}

	while(--i) {
		if(val[i] < '0' || val[i] > '9') {
			return FALSE;
		}
	}
	return TRUE;
}

#define HEX "ABCDEF"
static string int_to_hex(int v)
{
	string s;
	int *a;
	int i;

	s = "";
	i = 0;
	a = allocate(32);
	do {
		a[i] = v % 16;
		v /= 16;
		i++;
	} while(v);
	i--;

	while(i >= 0) {
		v = a[i];
		if(v > 9) {
			v -= 10;
			s += HEX[v..v];
		} else {
			s += v;
		}
		i--;
	}
	return s;
}

#define PAD "00000000"
static string int_to_bin(int v)
{
	string s;
	int i;

	s = "";
	i = 0;
	do {
		if(!(i % 8) && i) s = " " + s;
		s = (v % 2) + s;
		v /= 2;
		i++;
	} while(v);
	return PAD[0..7-strlen(explode(s, " ")[0])] + s;
}


static string pad_left(int final_length, string pad, string str)
{
	string tmp;
	if(strlen(pad) != 1) {
		error("invalid padding (" + strlen(pad) + ")");
	}

	if(strlen(str) >= final_length) {
		return str;
	}

	final_length -= strlen(str);
	tmp = "";
	while(strlen(tmp) < final_length) {
		tmp += pad;
	}
	return tmp + str;
}


static string hex_encode(string s)
{
	string ret;
	int i;

	ret = "";
	i = strlen(s);
	for(i = 0; i < strlen(s); i++) {
		ret += int_to_hex(s[i]);
	}
	return ret;
}


/** 
 * For lack of a better name.
 * - Takes str and generates an md5-hash
 * - Converts hash to a lower-cased hex-string
 * - Pads hex-string to a 32-byte string with leading 0's
 */
static string friendly_md5(string str)
{
	return pad_left(32, "0", lower_case(hex_encode(hash_md5(str))));
}


static string trim_left(string s)
{
	int i;
	i = 0;
	while(i < strlen(s) && s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
		i++;
	}
	return s[i..];
}

static string trim_right(string s)
{
	int i;
	i = strlen(s);
	while(i-- && s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
		/* Just smile */
	}
	return s[..i];
}

static string trim(string s)
{
	return trim_left(trim_right(s));
}

/**
 * TODO: Wtf? :) Was I drunk? Shouldn't it be enough with implode(explode())?
 */
static string replace(string in, string this, string with)
{
	if(!in || in == "" || strlen(with) > strlen(in))
		return in;

	if(with == "") {
		return implode(explode(in, this), with);
	}

	if(with == " ")
		in =  implode(explode("*" + in + "*", this), with);
	else
		in =  implode(explode(" " + in + " ", this), with);
	return in[1..strlen(in)-2];
}

static string strip_crlf(string str)
{
	str = replace(str, "\r\n", "\n");
	str = replace(str, "\r", "\n");
	return str;
}

static string normalize_whitechars(string str)
{
	str = replace(str, "\n", " ");
	str = replace(str, "\t", " ");
	return str;
}

static string strip_whitechars(string str)
{
	str = replace(str, "\n", "");
	str = replace(str, " ", "");
	str = replace(str, "\t", "");
	return str;
}

static string entity(string str)
{
	str = replace(str, "&lt;",   "<");
	str = replace(str, "&gt;",   ">");
	str = replace(str, "&quot;", "\"");
	str = replace(str, "&apos;", "\'");
	str = replace(str, "&amp;",  "&");
	return str;
}
																				
static string unentity(string str)
{
	if(!str || str == "")
		return str;

	str = replace(str, "&",  "&amp;");
	str = replace(str, "<",  "&lt;");
	str = replace(str, ">",  "&gt;");
	str = replace(str, "\"", "&quot;");
	str = replace(str, "\'", "&apos;");
	return str;
}

static int starts_with(string str, string cmp)
{
	if(!str || strlen(str) < strlen(cmp) || str[0..strlen(cmp)-1] != cmp) 
		return FALSE;
	return TRUE;
}

static int ends_with(string str, string cmp)
{
	if(!str || strlen(str) < strlen(cmp) 
			|| str[strlen(str)-strlen(cmp)..] != cmp)
		return FALSE;
	return TRUE;
}


/*
 * Integer to string.
 */
static string chr(int c)
{
	string s;
	s = " ";
	s[0] = c;
	return s;
}

/*
 * Character to integer
 */
static int asc(string a)
{
	return a[0];
}


/**
 * TODO: Rewrite this function as it is just down-right nasty.
 */
#define B64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
static string base64_decode(string s)
{
	string res, groupbin, group64, b1, b2, b3;
	int c1, c2, c3, c4, pos, i;

	if(strlen(s) % 4 > 0) {
		error("Bad base64 string: " + s + ".\n");
	}
	res = "";
	pos = 0;
	for(pos = 0; pos < strlen(s); pos+=4) {
		groupbin = "";
		group64 = s[pos..pos+3];
		c1 = index_of(0, B64, group64[0..0]);
		c2 = index_of(0, B64, group64[1..1]);
		c3 = index_of(0, B64, group64[2..2]);
		c4 = index_of(0, B64, group64[3..3]);
		b1 = chr(((c2 & 48) / 16) | (c1 * 4) & 0xff);
		b2 = groupbin + chr(((c3 & 60) / 4) | (c2 * 16) & 0xff);
		b3 = chr((((c3 & 3) * 64) & 0xff) | (c4 & 63));
		groupbin = b1 + b2 + b3;
		res += groupbin;
	}
	return ends_with(s, "=") ? res[..strlen(res)-2] : res;
}

static mapping parse_avp(string avps, string pair_sep, string avp_sep)
{
	mapping avp;
	string *pairs, *pair;
	int i;

	if(!avps || !strlen(avps)) {
		return nil;
	}
	avp = ([ ]);
	avps = trim(avps);
	pairs = explode(avps, pair_sep);
	for(i = 0; i < sizeof(pairs); i++) {
		int j, len;
		string attr, val;

		if(pairs[i] == "") {
			continue;
		}

		pair = explode(pairs[i], avp_sep);
		len = sizeof(pair);

		for(j = 0; j < len; j++) {
			pair[j] = trim(pair[j]);
		}

		attr = lower_case(pair[0]);

		if(len == 2)
			val = pair[1];
		else if(len == 1)
			val = "";
		else if(len > 2)
			val = implode(pair[1..], avp_sep);

		if(strlen(val) && val[0] == '"' && val[strlen(val)-1] == '"') {
			if(strlen(val) < 3) {
				/* malformed avp, continue and hope for the best */
				val = "";
			} else {
				val = val[1..strlen(val)-2];
			}
		}
		avp[attr] = val;
	}
	return avp;
}


# define SPACES "                                                                               "
static string ralign(mixed str, int size)
{
	return SPACES[0..(size-strlen(""+str))-1] + str;
}


/*
 * NAME:	normalize_path()
 * DESCRIPTION:	reduce a path to its minimal absolute form
 */
/* Shamefully stolen from DGD's Kernel Library for Igor compatibility */
static string normalize_path(string file, string dir, varargs string creator)
{
    string *path;
    int i, j, sz;

    if (strlen(file) == 0) {
	file = dir;
    }
    switch (file[0]) {
    case '~':
	/* ~path */
	if (creator && (strlen(file) == 1 || file[1] == '/')) {
	    file = USR + "/" + creator + file[1 ..];
	} else {
	    file = USR + "/" + file[1 ..];
	}
	/* fall through */
    case '/':
	/* absolute path */
	if (sscanf(file, "%*s//") == 0 && sscanf(file, "%*s/.") == 0) {
	    return file;	/* no changes */
	}
	path = explode(file, "/");
	break;

    default:
	/* relative path */
	if (sscanf(file, "%*s//") == 0 && sscanf(file, "%*s/.") == 0 &&
	    sscanf(dir, "%*s/..") == 0) {
	    /*
	     * simple relative path
	     */
	    return dir + "/" + file;
	}
	/* fall through */
    case '.':
	/*
	 * complex relative path
	 */
	path = explode(dir + "/" + file, "/");
	break;
    }

    for (i = 0, j = -1, sz = sizeof(path); i < sz; i++) {
	switch (path[i]) {
	case "..":
	    if (j >= 0) {
		--j;
	    }
	    /* fall through */
	case "":
	case ".":
	    continue;
	}
	path[++j] = path[i];
    }

    return "/" + implode(path[.. j], "/");
}
