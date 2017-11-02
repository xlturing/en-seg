/*
 * Tokenize.cc
 *
 *  Created on: 2017-10-31
 *      Author: litaoxiao
 */

#include "Tokenize.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#define MOD_TRACENAME L"TOKENIZER"
#define MOD_TRACECODE TOKEN_TRACE

std::set<std::wstring> Tokenize::abrevs;
std::list<std::pair<std::wstring, RegExps> > Tokenize::rules;
std::map<std::wstring, int> Tokenize::matches;

Tokenize::Tokenize() {
}
Tokenize::~Tokenize() {
}
void Tokenize::init_tokenize(const string& dat_path) {
	// load and store abreviations file.
	// At each iteration, a line content is read, and interpreted according to current state.
	list<pair<wstring, wstring> > macros;
	bool rul = false;
	wifstream tokenizer_dat(dat_path.c_str());
	std::locale loc("");
	tokenizer_dat.imbue(loc);
	wstring line, cur_section;
	while (getline(tokenizer_dat, line)) {
		if (line == L"")
			continue;
		if (line.at(0) == '#')
			continue;
		if (line.at(0) == '<' && line.at(1) == '/') { // section end tag
			cur_section = L"";
			continue;
		}
		if (line == L"<Macros>") {
			cur_section = L"Macros";
			continue;
		} else if (line == L"<RegExps>") {
			cur_section = L"RegExps";
			continue;
		} else if (line == L"<Abbreviations>") {
			cur_section = L"Abbreviations";
			continue;
		}
		wistringstream sin;
		sin.str(line);
		if (cur_section == L"Macros") { // reading macros.
			// reading macros
			wstring mname, mvalue;
			sin >> mname >> mvalue;
			macros.push_back(make_pair(mname, mvalue));
		} else if (cur_section == L"RegExps") { // reading regexps (tokenization rules). Expect section end or a rule
			wstring comm, re;
			unsigned int substr;
			// read tokenization rule
			sin >> comm >> substr >> re;
			// at least a rule found
			rul = true;
			// check all macros
			for (list<pair<wstring, wstring> >::iterator i = macros.begin(); i != macros.end(); i++) {
				wstring mname = L"{" + i->first + L"}";
				wstring mvalue = i->second;
				// replace all macro occurrences in the rule with its value
				wstring::size_type p = re.find(mname, 0);
				while (p != wstring::npos) {
					//re=re.substr(0,p)+mvalue+re.substr(p+mname.length());
					re.replace(p, mname.length(), mvalue);
					p = re.find(mname, p);
				}
			}
			RegExps x(re);
			rules.push_back(make_pair(comm, x));
			// create and store Regexp rule in rules vector.
			matches.insert(make_pair(comm, substr));
		} else if (cur_section == L"Abbreviations") { // reading abbreviations. Expect section end or an abbreviation
			// store abbreviation
			abrevs.insert(line);
		}
	}
	cout << "rules size: " << rules.size() << " matches size: " << matches.size() << " abrevs size: " << abrevs.size()
			<< endl;
	tokenizer_dat.close();
}

void Tokenize::tokenize(const std::wstring &p, list<wstring> &v) const {
	wstring t[10];
	list<pair<wstring, RegExps> >::const_iterator i;
	bool match;
	int j, substr, len = 0;
	vector<wstring> results;  // to store match results

	v.clear();
	// Loop until line is completely processed.
	wstring::const_iterator c = p.begin();
	while (c != p.end()) {
		// find first non-white space and erase leading whitespaces
		while (c != p.end() and iswspace(*c)) {
			++c;
		}
		// there where whitespaces at the end of the line.
		if (c == p.end())
			break;
		// find first matching rule
		match = false;
		for (i = rules.begin(); i != rules.end() && !match; i++) {
			try {
				if (i->second.search(c, p.end(), results, true)) {
					// regexp matches, extract substrings
					match = true;
					len = 0;
					substr = matches.find(i->first)->second;
					for (j = (substr == 0 ? 0 : 1); j <= substr && match; j++) {
						// get each requested  substring
						t[j] = results.at(j);
						len += t[j].length();
						// if special rule, match must be in abbrev file
						if ((i->first)[0] == L'*') {
							wstring lower = t[j];
							transform(lower.begin(), lower.end(), lower.begin(), towlower);
							if (abrevs.find(lower) == abrevs.end()) {
								match = false;
							}
						}
					}
				}
			} catch (...) {
				// boost::regexp rejects to match an expression if the matched string is too long
//				cout << L"Match too long for boost buffer: Rule " + i->first + L" skipped." << endl;
				cout << L"Provided input doesn't look like text." << endl;
			}
		}

		if (match) {
			i--;
			// create word for each matched substring and append it to token list
			substr = matches.find(i->first)->second;
			for (j = (substr == 0 ? 0 : 1); j <= substr; j++) {
				if (t[j].length() > 0) {
					v.push_back(t[j]);
				}
			}
			// remaining substring
			c += len;
		} else if (c != p.end()) {
//			cout
//					<< L"No rule matched input substring '" + wstring(c, p.end()) + L"'. Character '" + wstring(1, *c)
//							+ L"' skipped. Check your tokenization rules." << endl;
			c++;
		}
	}
}
