
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#else
	#include <limits.h>
#endif

#include <stdarg.h>

#include <utils.h>

#include <xml.h>

/**********************************************************************
	constructor
**********************************************************************/

XML::XML(const char *_fname, Logger *_logg)  {
	strncpy(fname, _fname, sizeof(fname)-1);
	strncpy(path, _fname, sizeof(fname)-1);
	strip_filename(path);

	logg = _logg;

	init();

#ifdef _DEBUG
	if (error_string[0]!=0)  {
		bp = 1;
	}
	else  {
		bp = 2;
	}
#endif

	return;
}

#if 0
/**********************************************************************
	default constructor
**********************************************************************/

XML::XML(void)  {
	bp = 0;
}

/**********************************************************************
	copy constructor
**********************************************************************/

XML::XML(const XML& copy)  {
	bp = 0;
}
#endif

/**********************************************************************
	destructor
**********************************************************************/

XML::~XML()  {
	destroy();
}

/**********************************************************************

**********************************************************************/

void XML::destroy(void)  {
	bp = 0;
	DEL(dochandle);
	DEL(doc);
	return;
}

/**********************************************************************

**********************************************************************/

int XML::init(void)  {
	bp = 0;
	memset(error_string, 0, sizeof(error_string));
	errptr = NULL;
	dochandle = 0;
	memset(buf, 0, sizeof(buf));
	memset(str, 0, sizeof(str));
	root_element = NULL;
	e = NULL;
	titex = NULL;
	value = NULL;
	text = NULL;

	doc = new TiXmlDocument(fname);

	loadOkay = doc->LoadFile();

	if ( !loadOkay )  {
		sprintf( error_string, "Could not load %s. Error='%s' at %d, %d\n", fname, doc->ErrorDesc(), doc->ErrorRow(), doc->ErrorCol() );
		errptr = error_string;
		return 1;
	}

	root_element = doc->FirstChildElement();
	if (!root_element)  {
		sprintf(error_string, "Cannot find 'root'\n");
		errptr = error_string;
		return 1;
	}

	dochandle = new TiXmlHandle(doc);
	if (!dochandle)  {
		sprintf(error_string, "Cannot find dochandle\n");
		errptr = error_string;
		return 2;
	}

	#ifdef WIN32
		_ASSERTE(_CrtCheckMemory());
	#endif

	return 0;
}

/**********************************************************************

**********************************************************************/

const char * XML::gettext(const char *_value)  {

	e = root_element->FirstChildElement();

	while(e)  {
		value = e->Value();
		text = e->GetText();
		//TiXmlNode node = e->ti

		if (!strcmp(value, _value))  {
			return text;
		}

#if 1
		if(myindex(buf, "sites") !=-1 )  {
			bp = 1;
		}
		else if(myindex(buf, "riders") !=-1 )  {
			bp = 2;
		}
#endif
		e = e->NextSiblingElement();
	}

	return NULL;
}

/**********************************************************************

**********************************************************************/

TiXmlElement * XML::getelement(const char *_value)  {
	int cnt = 0;

	e = root_element->FirstChildElement();

	while(e)  {
		cnt++;
		value = e->Value();
		text = e->GetText();
		if (!strcmp(value, _value))  {
			return e;
		}

#if 1
		if(myindex(value, "sites") !=-1 )  {
			bp = 1;
		}
		else if(myindex(value, "riders") !=-1 )  {
			bp = 2;
		}
#endif
		e = e->NextSiblingElement();
	}

	return NULL;
}

// ----------------------------------------------------------------------
// dump and indenting utility functions
// ----------------------------------------------------------------------

/**********************************************************************

**********************************************************************/


//const unsigned int NUM_INDENTS_PER_SPACE = 3;

const char * XML::getIndent( unsigned int numIndents )   {
#if 1
	unsigned int i;
	static char str[256];
	str[0] = 0;

	for(i=0; i<numIndents; i++)  {
		strcat(str, "   ");
	}
	return str;

#else
	unsigned int n;

	static const char *indents = "                                      + ";

	static const unsigned int len = strlen(indents);

	n = numIndents * NUM_INDENTS_PER_SPACE;

	if (n>len)  {
		n = len;
	}

	return &indents[len-n];
#endif

}

/**********************************************************************

**********************************************************************/

int XML::dump_attribs(TiXmlElement* pElement, unsigned int indent)  {
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	//const char* pIndent=getIndentAlt(indent);
	
	//logg->write("\n");
	logg->write("   ");

	while (pAttrib)  {
		logg->write( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    logg->write( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) logg->write( " d=%1.1f", dval);
		logg->write( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}

/**********************************************************************

**********************************************************************/

void XML::dump( TiXmlNode *parent, int level)  {
	int num, t;
	TiXmlNode *child;
	TiXmlText *text;


	if (!parent)  {
		return;
	}

	t = parent->Type();
	const char *cptr = getIndent(level);
	logg->write( "%s", cptr );

	switch (t)  {
		case TiXmlNode::DOCUMENT:
			logg->write( "Document" );
			break;

		case TiXmlNode::ELEMENT:
			logg->write( "%s", parent->Value() );
			num = dump_attribs(parent->ToElement(), level+1);

			switch(num)  {
				case 0:
					//logg->write( " (No attributes)"); 
					break;
				case 1:
					logg->write( "%s1 attribute", getIndent(level)); 
					break;
				default:
					logg->write( "%s%d attributes", getIndent(level), num); 
					break;
			}
			break;

		case TiXmlNode::COMMENT:
			logg->write( "%s", parent->Value());
			break;

		case TiXmlNode::UNKNOWN:
			logg->write( "Unknown" );
			break;

		case TiXmlNode::TEXT:
			text = parent->ToText();
			logg->write( "%s", text->Value() );
			break;

		case TiXmlNode::DECLARATION:
			logg->write( "Declaration" );
			break;

		default:
			break;
	}									// switch(t)


	logg->write( "\n" );

	for (child=parent->FirstChild(); child!=0; child=child->NextSibling())  {
		dump(child, level+1);											// recursive
	}

	return;
}						// dump( TiXmlNode* pParent, unsigned int indent)

/**********************************************************************
	load the named file and dump its structure
	http://www.grinninglizard.com/tinyxmldocs/tutorial0.html

**********************************************************************/

#define XY99 "xy99.xml"

void XML::dump(const char *_debugdir)  {

	if (!loadOkay)  {
		return;
	}
	if (!logg)  {
		return;
	}

	strcpy(str, _debugdir);
	strcat(str, "\\");
	strcat(str, XY99);

	save(str);
	logg->write("\n");
	logg->import_file(str);
	unlink(str);
	//logg->flush();
	logg->write(10,0,1,"\n");


	return;
}

/**********************************************************************
	load the named file and dump its structure
	http://www.grinninglizard.com/tinyxmldocs/tutorial0.html

**********************************************************************/

void XML::dump(void)  {

	if (!loadOkay)  {
		return;
	}
	if (!logg)  {
		return;
	}

	logg->write("\n%s:\n\n", fname);
	dump(doc);
	logg->write(10, 0, 1, "\n");

}

/**********************************************************************

**********************************************************************/
#if 1
const char *XML::get_text(int n, ...)  {
	va_list arguments;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;

	if (n==0)  {
		return NULL;
	}

	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return NULL;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return NULL;
	}

	e = h.ToElement();
	cptr = e->GetText();
	return cptr;
}

#else
//const char *XML::get_text(const char *v, ...)  {
const char *XML::get_text(std::vector<std::string> v)  {

	int n = v.size();
	int bp = 2;
	TiXmlElement *e=NULL;

	if (n==0)  {
		return NULL;
	}

	char query[256];

	strcpy(query, "dochandle->");
	for(i=0; i<n; i++)  {
		sprintf(str, "FirstChild(\"%s\")", v[i]);
		strcat(query, str);
	}

	e = dochandle->FirstChild( "options" ).FirstChild( "video_name" ).ToElement();
	e = dochandle->FirstChild( "options" ).FirstChild( "video_name" ).ToElement();

	return e->GetText();

/*
	va_list	ap;													// Argument pointer
	va_start(ap, v);
	vsprintf(str, v, ap);
	va_end(ap);
	return NULL;
*/

}
#endif

/**********************************************************************

**********************************************************************/

void XML::set_text(const char *_txt, int n, ...)  {

	va_list args;
	int i;
	//const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;

	if (n==0)  {
		return;
	}

#if 1
	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);


	h = dochandle->FirstChild(strs[0].c_str());
	child = h.ToNode();
	if (!child)  {
		return;
	}

#ifdef _DEBUG
	e = child->ToElement();
	value = e->Value();							// "options"
	text = e->GetText();						// NULL
#endif


	for (i=1; i<n; i++)  {
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			break;
		}

#ifdef _DEBUG
		e = child->ToElement();
		value = e->Value();							// "video_name"
		text = e->GetText();						// NULL
#endif

	}

	if (i != n)  {
		return;
	}

#ifdef _DEBUG
		value = e->Value();							// "video_name"
		text = e->GetText();						// NULL
		if (text==NULL)  {
			bp = 1;
		}
		e = child->ToElement();
		value = e->Value();							// "video_name"
		text = e->GetText();						// NULL
#endif

		text = e->GetText();						// NULL
		if (text==NULL)  {
			//set_element_text(e, _txt);
		}

		set_element_text(e, _txt);
#ifdef _DEBUG
		value = e->Value();							// "video_name"
		text = e->GetText();						// NULL
#endif

	/*
	e = h.ToElement();
	cptr = e->GetText();
	e->SetValue(text);					// error? see set_element_text()
	cptr = e->GetText();
	*/

#else


	va_start(args, n );           

	h = dochandle->FirstChild(va_arg(args, const char *));
	child = h.ToNode();
	if (!child)  {
		return;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(args, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (args);

	if (i != n)  {
		return;
	}

	e = h.ToElement();
	cptr = e->GetText();
	e->SetValue(text);					// error? see set_element_text()
	cptr = e->GetText();
#endif

	return;
}								// set_text()

/**********************************************************************

**********************************************************************/

void XML::set_int(int val, int n, ... )  {
	va_list args;
	int i, status;
	//const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlHandle hsave=NULL;
	TiXmlNode *child, *node;

	if (n==0)  {
		return;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	child = h.ToNode();
	if (!child)  {
		// if top level tag doesn't exist, create it
		TiXmlElement e(strs[0].c_str());
		//e.SetAttribute( "name", "Marple" );					// <test name="Marple" />
		node = dochandle->ToNode()->InsertEndChild(e);
		if (node==NULL)  {
			return;
		}
		h = dochandle->FirstChild(strs[0].c_str());
		child = h.ToNode();
		if (!child)  {
			return;
		}
	}

#ifdef _DEBUG
	e = child->ToElement();
	value = e->Value();							// "options"
	text = e->GetText();						// null
#endif


	for (i=1; i<n; i++)  {
		hsave = h;
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			TiXmlElement e(strs[i].c_str());
			node = hsave.ToNode()->InsertEndChild(e);
			if (node==NULL)  {
				return;
			}
			h = hsave.FirstChild(strs[i].c_str());
			child = h.ToNode();
			if (!child)  {
				return;
			}
		}

#ifdef _DEBUG
		e = child->ToElement();
		value = e->Value();							// "scale"
		text = e->GetText();						// "0"
#endif

	}				// for(i)


	if (i != n)  {
		return;
	}

	e = child->ToElement();


#ifdef _DEBUG
	value = e->Value();							// "pix"
#endif

	text = e->GetText();						// NULL

	sprintf(str, "%d", val);
//xxx
	if (text)  {
		e->Clear();
	}

	set_element_text(e, str);

	return;
}								// set_int()


/**********************************************************************

**********************************************************************/

int XML::get_int(int n, ...)  {
	va_list arguments;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;

	if (n==0)  {
		return INT_MIN;
	}

	if (!dochandle)  {
		return INT_MIN;
	}

	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return 0;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return 0;
	}

	e = h.ToElement();
	cptr = e->GetText();
	int nn, val;
	nn = sscanf(cptr, "%d", &val);
	if (nn != 1)  {
		return 0;
	}
	return val;
}														// get_int()


/**********************************************************************

**********************************************************************/

int XML::get_child_count(int n, ...)  {
	va_list arguments;
	int i;
	//const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;

	if (n==0)  {
		return 0;
	}

	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return 0;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return 0;
	}

/*
	e = h.ToElement();
	cptr = e->GetText();
	int nn, val;
	nn = sscanf(cptr, "%d", &val);
	if (nn != 1)  {
		return 0;
	}
*/
	int cnt = 0;
	for (child=child->FirstChild(); child!=0; child=child->NextSibling())  {
		cnt++;
	}

	return cnt;
}

/**********************************************************************

**********************************************************************/

void XML::remove_child(int which, int n, ...)  {
	va_list args;
	int i;
	//const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child, *child2;

	if (n==0)  {
		return;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	child = h.ToNode();
	if (!child)  {
		return;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	if (i != n)  {
		return;
	}

#ifdef _DEBUG
	e = child->ToElement();
	value = e->Value();							// "sites"
	text = e->GetText();						// NULL
#endif

	int cnt=0;
	for (child2=child->FirstChild(); child2!=0; child2=child2->NextSibling())  {

#ifdef _DEBUG
		e = child2->ToElement();
		value = e->Value();
		text = e->GetText();
#endif

		if (cnt==which)  {
			child->RemoveChild(child2);
			break;
		}
		cnt++;
	}


	/*
	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return;
	}
	*/

/*
	e = h.ToElement();
	cptr = e->GetText();
	int nn, val;
	nn = sscanf(cptr, "%d", &val);
	if (nn != 1)  {
		return 0;
	}
*/

	/*
	int cnt=0;
	for (child=child->FirstChild(); child!=0; child=child->NextSibling())  {
		if (cnt==which)  {
			child->RemoveChild(child);
			break;
		}
		cnt++;
	}
	*/

	return;
}

/**********************************************************************

**********************************************************************/

double XML::get_double(int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;
	int nn;
	double d;

	if (n==0)  {
		return 0.0;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

#if 1
	h = dochandle->FirstChild(strs[0].c_str());
	child = h.ToNode();
	if (!child)  {
		return 0.0;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	if (i != n)  {
		return 0.0;
	}

#else
	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return NULL;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return NULL;
	}
#endif

	e = h.ToElement();
	cptr = e->GetText();
	nn = sscanf(cptr, "%lf", &d);
	if (nn != 1)  {
		return 0.0f;
	}
	return d;

}														// get_double()

/**********************************************************************
	example:
	xml->set_double(-9.0, 2, "options", "scale");

**********************************************************************/

void XML::set_double(double d, int n, ... )  {
	va_list args;
	int i;
	//const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child, *node;
	TiXmlHandle hsave=NULL;

	if (n==0)  {
		return;
	}

#if 1
	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	child = h.ToNode();
	//if (!child)  {
	//	return;
	//}

	if (!child)  {
		// if top level tag doesn't exist, create it
		TiXmlElement e(strs[0].c_str());
		//e.SetAttribute( "name", "Marple" );					// <test name="Marple" />
		node = dochandle->ToNode()->InsertEndChild(e);
		if (node==NULL)  {
			return;
		}
		h = dochandle->FirstChild(strs[0].c_str());
		child = h.ToNode();
		if (!child)  {
			return;
		}
	}


#ifdef _DEBUG
	e = child->ToElement();
	value = e->Value();							// "options"
	text = e->GetText();						// null
#endif


	/*
	for (i=1; i<n; i++)  {
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	if (i != n)  {
		return;
	}
	*/

	for (i=1; i<n; i++)  {
		hsave = h;
		h = h.FirstChild(strs[i].c_str());
		child = h.ToNode();
		if (!child)  {
			TiXmlElement e(strs[i].c_str());
			node = hsave.ToNode()->InsertEndChild(e);
			if (node==NULL)  {
				return;
			}
			h = hsave.FirstChild(strs[i].c_str());
			child = h.ToNode();
			if (!child)  {
				return;
			}
		}

#ifdef _DEBUG
		e = child->ToElement();
		value = e->Value();							// "scale"
		text = e->GetText();						// "0"
#endif

	}			// for(i)

	if (i != n)  {
		return;
	}

	e = child->ToElement();

#ifdef _DEBUG
	value = e->Value();							// "pix"
#endif

	text = e->GetText();						// NULL
	sprintf(str, "%f", (float)d);

//xxx

	if (text)  {
		e->Clear();
	}

	set_element_text(e, str);

/*
	//cptr = e->set
	cptr = e->GetText();
	sprintf(str, "%f", d);
	e->SetValue(str);
	cptr = e->GetText();
	set_element_text(e, str);
	cptr = e->GetText();
*/

#else
	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return;
	}

	e = h.ToElement();

	//cptr = e->set
	cptr = e->GetText();

	sprintf(str, "%lf", d);
	e->SetValue(str);
	cptr = e->GetText();
#endif

	return;
}											// set_double()

/**********************************************************************

**********************************************************************/

void XML::set_double(int ix, double d, int n, ... )  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *child;
	TiXmlNode *node, *node2, *foundnode=NULL;

	if (n==0)  {
		return;
	}

#if 1
	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	if (!h.ToNode())  {
		return;
	}

	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return;
		}
	}

	if (i != n-1)  {
		return;
	}

	int cnt=0;

	node = h.ToNode();

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}

	if (!foundnode)  {
		node2 = h.ToNode();

		node2 = node2->FirstChild();
		const char *cptr2 = node2->Value();				// site
		i = add_child(3, strs[0].c_str(), strs[1].c_str(), cptr2);
		if (i != 0)  {
			return;
		}
		foundnode = h.ToNode()->LastChild();					// get the node that we just added
	}

	cptr = strs[n-1].c_str();
	node2 = foundnode->FirstChild(cptr);

	if (node2)  {
		// element already exists
		e = node2->ToElement();
		if(e==NULL)  {
			return;
		}
		child = e->FirstChild();
		sprintf(str, "%f", d);
		child->SetValue(str);
		bp = 1;
	}
	else  {
		// element doesn't exist
		cptr = strs[n-1].c_str();					// "name"
		TiXmlElement child(cptr);					// create the "name" element
	
		node = foundnode->InsertEndChild(child);	// adds the name element under 'site', eg
		if (node==NULL)  {
			return;
		}

		// node is the name element

		e = node->ToElement();							// e is the node element
		sprintf(str, "%f", d);
		titex = new TiXmlText(str);
		e->LinkEndChild(titex);
	
	}

#else
	va_start(arguments, n );           

	h = dochandle->FirstChild(va_arg(arguments, const char *));
	child = h.ToNode();
	if (!child)  {
		return;
	}

	for (i=1; i<n; i++)  {
		h = h.FirstChild(va_arg(arguments, const char *));
		child = h.ToNode();
		if (!child)  {
			break;
		}
	}
	va_end (arguments);

	if (i != n)  {
		return;
	}

	e = h.ToElement();

	//cptr = e->set
	cptr = e->GetText();

	sprintf(str, "%lf", d);
	e->SetValue(str);
	cptr = e->GetText();
#endif

	return;
}											// set_double(ix,...)

/**********************************************************************

**********************************************************************/

const char * XML::get_text(int ix, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *foundnode=NULL;
	//int nn, val;

	if (n==0)  {
		return NULL;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	cptr = strs[0].c_str();							// "options"
	h = dochandle->FirstChild(cptr);
	if (!h.ToNode())  {
		return NULL;
	}

	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return NULL;
		}
		#ifdef _DEBUG
		node = h.ToNode();
		e = node->ToElement();
		value = e->Value();							// "videos"
		text = e->GetText();						// NULL
		#endif
	}
	node = h.ToNode();
	int cnt=0;

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		#ifdef _DEBUG
		e = node->ToElement();
		value = e->Value();							// "video"
		text = e->GetText();						// "C:\Users\larry\Documents\CompuTrainer TTS\Courses\NYCtri.avi"
		#endif

		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}


	if (foundnode)  {
		cptr = strs[n-1].c_str();
		node = foundnode->FirstChild(cptr);
		if (node==NULL)  {
			//save("test.xml");
			return NULL;
		}
		e = node->ToElement();
		if(e==NULL)  {
			return NULL;
		}
		return e->GetText();
	}
	else  {
		return NULL;
	}
}														// get_text()

/**********************************************************************

**********************************************************************/

void XML::save(const char *_fname)  {
	bool b;
	
	if (_fname==NULL)  {
		b = doc->SaveFile(fname);
	}
	else  {
		b = doc->SaveFile(_fname);
	}

	if (!b)  {
		bp = 2;
	}

	return;
}						// save()

/**********************************************************************

	eg, status = sxml->add_child(3, "options", "sites", "site");

**********************************************************************/

int XML::add_child(int n, ...)  {
	va_list args;
	//const char *cptr;
	int i;
	TiXmlHandle h=NULL;
	TiXmlNode *node;
	const char *cptr;


	if (n==0)  {
		return 1;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	if (!h.ToNode())  {
		return 2;
	}

	for(i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str() );
		if (!h.ToNode())  {
			h = dochandle->FirstChild(strs[0].c_str());
			cptr = strs[i].c_str();
			status = add_child(2, "options", cptr);
			h = h.FirstChild(strs[i].c_str() );
			if (!h.ToNode())  {
				return 3;
			}
		}
	}

	TiXmlElement child( strs[i].c_str() );
	//child.SetAttribute( "name", "Marple" );


	node = h.ToNode()->InsertEndChild(child);
	if (node==NULL)  {
		return 4;
	}

	return 0;
}							// add_child()


/**********************************************************************

**********************************************************************/

int XML::collect(std::vector<std::string> &strs, int n, va_list args )  {
	const char *cptr;
	int i;


	for(i=0; i<n; i++)  {
		cptr = va_arg(args, const char *);
		if (cptr==NULL)  {
			break;
		}

		strs.push_back(cptr);
	}

	return i;
}							// collect()

/**********************************************************************

**********************************************************************/

void XML::set_text(int ix, const char *txt, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *node2, *foundnode=NULL;
	//int nn, val;
	TiXmlNode *child;

	if (n==0)  {
		return;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	if (!h.ToNode())  {
		return;
	}

	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return;
		}
	}

	if (i != n-1)  {
		return;
	}

	int cnt=0;

	node = h.ToNode();

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}

	if (!foundnode)  {
		node2 = h.ToNode();

		node2 = node2->FirstChild();
		const char *cptr2 = node2->Value();				// site
		i = add_child(3, strs[0].c_str(), strs[1].c_str(), cptr2);
		if (i != 0)  {
			return;
		}
		foundnode = h.ToNode()->LastChild();					// get the node that we just added
	}

	cptr = strs[n-1].c_str();
	node2 = foundnode->FirstChild(cptr);

	if (node2)  {
		//  node already exists
		e = node2->ToElement();

		#ifdef _DEBUG
			//e = child->ToElement();
			value = e->Value();							// node2 = "name"
		#endif

		if(e==NULL)  {
			return;
		}

		#ifdef _DEBUG
			text = e->GetText();							// node2 text is "testing"
			if (text==NULL)  {
				bp = 3;
			}
		#endif

		child = e->FirstChild();

		if (child)  {

			#ifdef _DEBUG
				//e = child->ToElement();					// NULL
			#endif

			if (txt==NULL)  {
				node2->RemoveChild(child);
			}
			else if (txt[0]==0)  {
				node2->RemoveChild(child);
			}
			else  {
				child->SetValue(txt);
			}

#ifdef _DEBUG
			text = e->GetText();						// NULL
#endif

		}
		else  {
			//text between tags doesn't exist
			titex = new TiXmlText(txt);
			e->LinkEndChild(titex);
		}
	}
	else  {
		// element doesn't exist
		cptr = strs[n-1].c_str();					// "name"
		TiXmlElement child(cptr);					// create the "name" element
	
		node = foundnode->InsertEndChild(child);	// adds the name element under 'site', eg
		if (node==NULL)  {
			return;
		}

		// node is the name element

		e = node->ToElement();							// e is the node element
		titex = new TiXmlText(txt);
		e->LinkEndChild(titex);
	
	}

	return;
}														// set_text()

/**********************************************************************

**********************************************************************/

void XML::set_int(int ix, int ival, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *node2, *foundnode=NULL;
	//int nn, val;
	TiXmlNode *child;

	if (n==0)  {
		return;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	h = dochandle->FirstChild(strs[0].c_str());
	if (!h.ToNode())  {
		return;
	}


	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return;
		}
	}

	if (i != n-1)  {
		return;
	}

	int cnt=0;

	node = h.ToNode();

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}

	if (!foundnode)  {
		node2 = h.ToNode();
		node2 = node2->FirstChild();
		const char *cptr2 = node2->Value();				// site
		i = add_child(3, strs[0].c_str(), strs[1].c_str(), cptr2);
		if (i != 0)  {
			return;
		}

		foundnode = h.ToNode()->LastChild();					// get the node that we just added

	}

	cptr = strs[n-1].c_str();
	node2 = foundnode->FirstChild(cptr);

	//bool debug = false;

	if (node2)  {
		// element already exists
		e = node2->ToElement();
		if(e==NULL)  {
			return;
		}
		sprintf(str, "%d", ival);
		//set_element_text(e, str);
		child = e->FirstChild();
		child->SetValue(str);
	}

	else  {
		// element doesn't exist

		cptr = strs[n-1].c_str();					// "name"
		TiXmlElement child(cptr);					// create the "name" element
	
		node = foundnode->InsertEndChild(child);	// adds the name element under 'site', eg
		if (node==NULL)  {
			return;
		}

		// node is the name element
		sprintf(str, "%d", ival);
		TiXmlElement child2(str);

		e = node->ToElement();							// e is the node element
		titex = new TiXmlText(str);
		e->LinkEndChild(titex);

	}

	return;
}											// set_int()

/**********************************************************************

**********************************************************************/

int XML::get_int(int ix, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *foundnode=NULL;
	int nn, val, cnt;

	if (n==0)  {
		return 0;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	if (logg)  {
		for(i=0; i<(int)strs.size();i++)  {
			logg->write(10, 0, 1, "%d %s\n", i, strs[i].c_str());
		}
	}

	h = dochandle->FirstChild(strs[0].c_str());
	if (!h.ToNode())  {
		return 0;
	}

	if (logg) logg->write(10, 0, 1, "gi1\n");

	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		node = h.ToNode();
		if (!node)  {
			return 0;
		}

	}

	if (logg) logg->write(10, 0, 1, "gi2\n");
	cnt = 0;

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}

	if (logg) logg->write(10, 0, 1, "gi3\n");

	if (!foundnode)  {
		return 0;
	}

	cptr = strs[n-1].c_str();
	node = foundnode->FirstChild(cptr);
	if (!node)  {
		return 0;
	}

	if (logg) logg->write(10, 0, 1, "gi4\n");

	e = node->ToElement();
	if(e==NULL)  {
		return 0;
	}

	if (logg) logg->write(10, 0, 1, "gi5\n");

	cptr = e->GetText();
	nn = sscanf(cptr, "%d", &val);
	if (nn != 1)  {
		return 0;
	}
	if (logg) logg->write(10, 0, 1, "gi6, v = %d\n", val);

	return val;
}														// get_int()

/**********************************************************************

**********************************************************************/

void XML::set_element_text(TiXmlElement *e, const char *str)  {
		titex = new TiXmlText(str);
		e->LinkEndChild(titex);
		return;
}

#if 1

/**********************************************************************

**********************************************************************/

double XML::get_double(int ix, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *foundnode=NULL;
	int nn;
	double d;

	if (n==0)  {
		return 0.0;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	cptr = strs[0].c_str();							// "gpx"
	h = dochandle->FirstChild(cptr);
	if (!h.ToNode())  {
		return 0.0;
	}

	#ifdef _DEBUG
		node = h.ToNode();
		e = node->ToElement();
		value = e->Value();							// "gpx"
		text = e->GetText();						// NULL
	#endif

	for (i=1; i<n-1; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return 0.0;
		}
		#ifdef _DEBUG
			node = h.ToNode();
			e = node->ToElement();
			value = e->Value();							// "trk", "trkseg", "trkpt"
			text = e->GetText();						// NULL
		#endif
	}							// for()

	node = h.ToNode();

	#ifdef _DEBUG
		e = node->ToElement();
		value = e->Value();							// "trkpt"
		text = e->GetText();						// NULL
	#endif

	int cnt=0;

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}


	if (foundnode)  {
		#ifdef _DEBUG
			e = foundnode->ToElement();
			value = e->Value();							// "ele"
			text = e->GetText();						// 34.22...
		#endif

		cptr = strs[n-1].c_str();					// "kgs", "ele"
		node = foundnode->FirstChild(cptr);
		if (node==NULL)  {
			//save("test.xml");
			return 0.0;
		}
		e = node->ToElement();
		if(e==NULL)  {
			return 0.0;
		}

	#ifdef _DEBUG
		value = e->Value();							// "trkpt"
		text = e->GetText();						// NULL
	#endif

		cptr = e->GetText();
		nn = sscanf(cptr, "%lf", &d);
		if (nn==1)  {
			return d;
		}
		return 0.0;
	}
	else  {
		return 0.0;
	}
}														// get_double()

#else

/**********************************************************************

**********************************************************************/

double XML::get_double(int ix, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *foundnode=NULL;
	int nn, val;
	double d;
#ifdef _DEBUG
	//TiXmlNode *child;
#endif

	if (n==0)  {
		return NULL;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	/*
	<gpx version="1.1" creator="MapMyFitness.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://www.topografix.com/GPX/1/1" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd">
		<trk>
			<trkseg>

				<trkpt lat="40.860520680765" lon="-73.956037488415">
					<ele>34.223360311381</ele>
				</trkpt>

				<trkpt lat="40.860520680765" lon="-73.956037488415">
					<ele>34.223360311381</ele>
				</trkpt>

				....

				<trkpt lat="40.860520680765" lon="-73.956037488415">
					<ele>34.223360311381</ele>
				</trkpt>

			</trkseg>
		</trk>
	</gpx>
	*/

	cptr = strs[0].c_str();							// "gpx"
	h = dochandle->FirstChild(cptr);
	if (!h.ToNode())  {
		return 0.0;
	}

	#ifdef _DEBUG
		node = h.ToNode();
		e = node->ToElement();
		value = e->Value();							// "gpx"
		text = e->GetText();						// NULL
	#endif

	for (i=1; i<n-1; i++)  {
		cptr = strs[i].c_str();						// "trk"
		h = h.FirstChild(cptr);
		//if (!h)  {
		//	return 0.0;
		//}

		/*
		node = h.ToNode();
		if (!node)  {
			return 0.0;
		}

		#ifdef _DEBUG
			e = node->ToElement();
			value = e->Value();							// "trk"
			text = e->GetText();						// NULL
		#endif
		*/

	}

	node = h.ToNode();
	#ifdef _DEBUG
		e = node->ToElement();
		value = e->Value();							// "trk"
		text = e->GetText();						// NULL
	#endif

	int cnt=0;

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}


	if (foundnode)  {
		cptr = strs[n-1].c_str();
		node = foundnode->FirstChild(cptr);
		if (node==NULL)  {
			//save("test.xml");
			return NULL;
		}
		e = node->ToElement();
		if(e==NULL)  {
			return NULL;
		}
		cptr = e->GetText();
		nn = sscanf(cptr, "%lf", &d);
		if (nn==1)  {
			return d;
		}
		return 0.0;
	}
	else  {
		return 0.0;
	}
}														// get_double()

#endif				// #if 0


/**********************************************************************

**********************************************************************/

const char * XML::get_attribute(int ix, int n, ...)  {
	va_list args;
	int i;
	const char *cptr;
	TiXmlHandle h=NULL;
	TiXmlNode *node, *foundnode=NULL;
	//int nn, val;

	if (n==0)  {
		return NULL;
	}

	va_start(args, n );
		std::vector<std::string> strs;
		int status = collect(strs, n, args);
		assert(status==n);
	va_end (args);

	cptr = strs[0].c_str();							// "gpx"
	h = dochandle->FirstChild(cptr);
	if (!h.ToNode())  {
		return NULL;
	}

	#ifdef _DEBUG
		node = h.ToNode();
		e = node->ToElement();
		value = e->Value();							// "gpx"
		text = e->GetText();						// NULL
	#endif

	for (i=1; i<n-2; i++)  {
		h = h.FirstChild(strs[i].c_str());
		if (!h.ToNode())  {
			return NULL;
		}
		#ifdef _DEBUG
			node = h.ToNode();
			e = node->ToElement();
			value = e->Value();							// "trk", "trkseg"
			text = e->GetText();						// NULL
			bp = i;
		#endif
	}

	node = h.ToNode();

	#ifdef _DEBUG
		e = node->ToElement();
		value = e->Value();							// "trkseg"
		text = e->GetText();						// NULL
	#endif


	int cnt=0;

	for (node=node->FirstChild(); node!=0; node=node->NextSibling())  {
		if (cnt==ix)  {
			foundnode = node;
			break;
		}
		cnt++;
	}

	//int result;
	//double d;

	if (foundnode)  {
		#ifdef _DEBUG
			e = foundnode->ToElement();
			value = e->Value();							// "trkpt"
			text = e->GetText();						// NULL
		#endif

		cptr = strs[n-1].c_str();						// "lat"
		//result = e->QueryDoubleAttribute( cptr, &d );
		cptr = e->Attribute(cptr);						// "40.8"
		return cptr;

		/*
		// QueryIntAttribute()
		// FirstAttribute()
		// Attribute()

		node = foundnode->FirstChild(cptr);
		if (node==NULL)  {
			//save("test.xml");
			return NULL;
		}
		e = node->ToElement();
		if(e==NULL)  {
			return NULL;
		}
		return e->GetText();
		*/
	}
	else  {
		return NULL;
	}
}														// get_attribute()
