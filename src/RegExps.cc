/*
 * RegexP.cc
 *
 *  Created on: 2017-10-31
 *      Author: litaoxiao
 */

#include "RegExps.h"
#include <locale>
#include <iostream>

using namespace std;
#define CONTINUOUS boost::match_continuous
#define SEARCH boost::regex_search
#define MATCH boost::regex_match

RegExps::RegExps(const wstring &expr) {
	try {
		re.set_expression(expr);
	} catch (const boost::regex_error &e) {
		wcerr << L"Error compiling regular expression: " << expr << endl;
		throw e;
	}
}

RegExps::~RegExps() {
}

///////////////////////////////////////////////
/// Search for a partial match in a string
///////////////////////////////////////////////
bool RegExps::search(const wstring &in, bool continuous) const {
	if (continuous)
		return SEARCH(in.begin(), in.end(), re, CONTINUOUS);
		else
		return SEARCH(in.begin(), in.end(), re);
	}

///////////////////////////////////////////////
/// Search for a partial match in a string, return sub matches
///////////////////////////////////////////////
bool RegExps::search(const wstring &in, vector<wstring> &out, bool continuous) const {
	return this->search(in.begin(), in.end(), out, continuous);
}

///////////////////////////////////////////////
/// Search for a partial match in a string, return
/// sub matches and positions
///////////////////////////////////////////////
bool RegExps::search(const wstring &in, vector<wstring> &out, vector<int> &pos, bool continuous) const {
	return this->search(in.begin(), in.end(), out, pos, continuous);
}

///////////////////////////////////////////////
/// Search for a partial match in a string, return sub matches
///////////////////////////////////////////////
bool RegExps::search(wstring::const_iterator i1, wstring::const_iterator i2, vector<wstring> &out,
		bool continuous) const {
	match_type what;
	out.clear();

	bool ok = (continuous ? SEARCH(i1,i2,what,re,CONTINUOUS)
	: SEARCH(i1,i2,what,re));

	if (ok)
		extract_matches(what, out);
	return ok;
}

///////////////////////////////////////////////
/// Search for a partial match in a string, return sub matches and positions
///////////////////////////////////////////////
bool RegExps::search(wstring::const_iterator i1, wstring::const_iterator i2, vector<wstring> &out, vector<int> &pos,
		bool continuous) const {
	match_type what;
	out.clear();
	pos.clear();

	bool ok = (continuous ? SEARCH(i1,i2,what,re,CONTINUOUS)
	: SEARCH(i1,i2,what,re));

	if (ok)
		extract_matches(what, out, pos);
	return ok;
}

///////////////////////////////////////////////
/// Search for a whole match in a string
///////////////////////////////////////////////
bool RegExps::match(const wstring &in) const {
	return MATCH(in.begin(),in.end(),re);
}

///////////////////////////////////////////////
/// Search for a whole match in a string, return sub matches
///////////////////////////////////////////////
bool RegExps::match(const wstring &in, vector<wstring> &out) const {

	match_type what;
	out.clear();

	bool ok = MATCH(in.begin(),in.end(),what,re);
	if (ok)
		extract_matches(what, out);
	return ok;
}

///////////////////////////////////////////////
/// private function: convert internal match list to vector<string>
///////////////////////////////////////////////
void RegExps::extract_matches(const match_type &what, vector<wstring> &out) const {
	for (size_t i = 0; i < what.size(); ++i) {
		out.push_back(what.str(i));
	}
}

///////////////////////////////////////////////
/// private function: convert internal match list to vector<string> and their positions to vector<int>
///////////////////////////////////////////////
void RegExps::extract_matches(const match_type &what, vector<wstring> &out, vector<int> &pos) const {
	for (size_t i = 0; i < what.size(); ++i) {
		out.push_back(what.str(i));
		pos.push_back(what.position(i));
	}
}
