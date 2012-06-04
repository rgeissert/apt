
/**
 *  Copyright 2012 Raphael Geissert <geissert@debian.org>
 *
 *  HttpHeader parser based on code from APT 0.9.3 which has the
 *  following copyright notice:
 *  Apt is copyright 1997, 1998, 1999 Jason Gunthorpe and others.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


//#include <config.h>

#include <apt-pkg/strutl.h>

#include <string>
#include "http_header.h"

using namespace std;

#define SKIP_SPACES(string, pos) { \
	    while (pos < string.length() && isspace(string[pos]) != 0) \
		pos++; \
	    }

HttpHeader::HttpHeader() {
}

HttpHeader::HttpHeader(const string &HdrName, const std::string &HdrValue) {
    HeaderName = HdrName;
    HeaderValue = HdrValue;
}

HttpHeader::HttpHeader(const string &Header) {
    string::size_type Pos = Header.find(':');
    if (Pos == string::npos || (Pos + 2) > Header.length())
	return;

    // Parse off any trailing spaces between the : and the next word.
    string::size_type Pos2 = Pos + 1;
    SKIP_SPACES(Header, Pos2);

    HeaderName = Header.substr(0, Pos);
    HeaderValue = Header.substr(Pos2);

    // remove trailing white space
    string::size_type Pos3 = HeaderValue.length();
    while (Pos3 > 0 && isspace(HeaderValue[Pos3 - 1]))
	Pos3--;
    HeaderValue.resize(Pos3);

    return;
}

bool HttpHeader::empty() {
    return (HeaderName.empty() || HeaderValue.empty());
}

string HttpHeader::name() {
    return HeaderName;
}

string HttpHeader::value() {
    return HeaderValue;
}

vector<HttpHeader> HttpHeader::split() {
    vector<HttpHeader> HeadersVec;
    vector<string> ValuesVec;

    ValuesVec = VectorizeString(HeaderValue, ',');

    int VVecSz = ValuesVec.size();
    for (int i = 0; i < VVecSz; i++) {
	HttpHeader NewHeader = HttpHeader(HeaderName, ValuesVec[i]);
	if (!NewHeader.empty())
	    HeadersVec.push_back(NewHeader);
    }

    return HeadersVec;
}

HttpLinkHeader::HttpLinkHeader() {
}

HttpLinkHeader::HttpLinkHeader(HttpHeader NewHeader) {
    if (stringcasecmp(NewHeader.name(), "link") != 0)
	return;

    string value = NewHeader.value();
    if (value[0] != '<')
	return;

    string::size_type Pos = value.find('>');
    if (Pos == string::npos)
	return;

    URIRef = value.substr(1, Pos-1);

    Pos = value.find(';', Pos);
    if (Pos != string::npos) {
	do {
	    Pos++;
	    SKIP_SPACES(value, Pos);
	    string::size_type Pos2 = Pos;
	    while (Pos2 < value.length() && value[Pos2] != '='
		    && value[Pos2] != ';' &&
		    isspace(value[Pos2]) == 0)
		Pos2++;

	    string ParamKey, ParamVal;
	    ParamKey = value.substr(Pos, Pos2 - Pos);

	    SKIP_SPACES(value, Pos2);

	    if (Pos2 < value.length() && value[Pos2] != ';') {
		char delimiter = ';';

		Pos2++;
		SKIP_SPACES(value, Pos2);

		if (Pos2 < value.length() && value[Pos2] == '"') {
		    delimiter = '"';
		    Pos2++;
		}
		Pos = Pos2;

		while (Pos2 < value.length() && value[Pos2] != delimiter &&
			(delimiter != ';' || isspace(value[Pos2]) == 0))
		    Pos2++;
		if (Pos2 <= value.length()) {
		    ParamVal = value.substr(Pos, Pos2 - Pos);
		    // there must always be a ; except for when we
		    // reach the end of the header
		    Pos2 = value.find(';', Pos2);
		}
	    }
	    if (!ParamKey.empty())
		Params[ParamKey] = ParamVal;
	    Pos = Pos2;
	} while (Pos < value.length());
    }

    Header = NewHeader;
}

bool HttpLinkHeader::empty() {
    return Header.empty();
}

string HttpLinkHeader::getURI() {
    return URIRef;
}

bool HttpLinkHeader::has(string Key) {
    return (Params.count(Key) != 0);
}

string HttpLinkHeader::param(string Key) {
    return Params.find(Key)->second;
}

size_t HttpLinkHeader::paramsCount() {
    return Params.size();
}
