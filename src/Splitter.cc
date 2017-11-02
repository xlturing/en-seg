/*
 * Splitter.cc
 *
 *  Created on: 2017-11-01
 *      Author: litaoxiao
 */

#include "Splitter.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#define MOD_TRACENAME L"SPLITTER"
#define MOD_TRACECODE SPLIT_TRACE
#define SAME 100
#define VERY_LONG 1000

RegExps Splitter::RE_is_capitalized(L"^[[:upper:]]");
bool Splitter::SPLIT_AllowBetweenMarkers;
int Splitter::SPLIT_MaxWords;
std::set<std::wstring> Splitter::starters;
std::map<std::wstring, bool> Splitter::enders;
std::map<std::wstring, int> Splitter::markers;
Splitter::Splitter() {
}
Splitter::~Splitter() {
}

void Splitter::init_splitter(const std::string& dat_path) {
	// default values and initializations
	SPLIT_AllowBetweenMarkers = true;
	SPLIT_MaxWords = 0;
	wifstream splitter_dat(dat_path);
	std::locale loc("");
	splitter_dat.imbue(loc);
	// process each content line according to the section where it is found
	int nmk = 1;
	wstring line, cur_section;
	while (getline(splitter_dat, line)) {
		if (line == L"")
			continue;
		if (line.at(0) == '#')
			continue;
		if (line.at(0) == '<' && line.at(1) == '/') { // section end tag
			cur_section = L"";
			continue;
		}
		if (line == L"<General>") {
			cur_section = L"General";
			continue;
		} else if (line == L"<Markers>") {
			cur_section = L"Markers";
			continue;
		} else if (line == L"<SentenceEnd>") {
			cur_section = L"SentenceEnd";
			continue;
		} else if (line == L"<SentenceStart>") {
			cur_section = L"SentenceStart";
			continue;
		}
		wistringstream sin;
		sin.str(line);
		if (cur_section == L"General") {
			// reading general options
			wstring name;
			sin >> name;
			if (name == L"AllowBetweenMarkers")
				sin >> SPLIT_AllowBetweenMarkers;
			else if (name == L"MaxWords")
				sin >> SPLIT_MaxWords;
			//			else
			//				cerr << L"Unexpected splitter option '" + name + L"'" << endl;
		} else if (cur_section == L"Markers") {
			wstring open, close;
			// reading open-close marker pairs (parenthesis, etc)
			sin >> open >> close;
			if (open != close) {
				markers.insert(make_pair(open, nmk));   // positive for open, negative for close
				markers.insert(make_pair(close, -nmk));
			} else {  // open and close are the same (e.g. double "quotes")
				markers.insert(make_pair(open, SAME + nmk));   // both share the same code, but its distinguishable
				markers.insert(make_pair(close, SAME + nmk));
			}
			nmk++;
		} else if (cur_section == L"SentenceEnd") {
			// reading end-sentence delimiters
			wstring name;
			bool value;
			sin >> name >> value;
			enders.insert(make_pair(name, value));
		} else if (cur_section == L"SentenceStart") {
			// reading start-sentence delimiters
			starters.insert(line);
		}
	}
	cout << "starters size: " << starters.size() << " enders size: " << enders.size() << " markers size: "
			<< markers.size() << endl;
}

///////////////////////////////////////////////////////////////
///  Open a session, and create a copy of the internal status for it.
///  Sessions are needed in case the same splitter is used
///  to split different files simultaneously (either by the same
///  thread or by different threads
///////////////////////////////////////////////////////////////
Splitter::session_id Splitter::open_session() const {
	splitter_status *ss = new splitter_status();
	ss->betweenMrk = false;
	ss->no_split_count = 0;
	ss->nsentence = 0;
	return (ss);
}

///////////////////////////////////////////////////////////////
///  Accumulate the word list v into the internal buffer.
///  If a sentence marker is reached (or flush flag is set),
///  return all sentences currently in buffer, and clean buffer.
///  If a new sentence is started but not completed, keep
///  in buffer, and wait for further calls with more data.
///////////////////////////////////////////////////////////////
void Splitter::split(session_id st, const list<std::wstring> &v, bool flush, list<list<std::wstring>> &ls) const {
// clear list of sentences from previous use
	ls.clear();
	for (list<std::wstring>::const_iterator w = v.begin(); w != v.end(); w++) {
		// check whether we found a marker (open/close parenthesis, quote, etc)
		map<wstring, int>::const_iterator m = markers.find(*w);
		bool check_split = true;
		// last opened marker is being closed, pop from stack, if
		// stack is empty, we are no longer inbetween markers
		if (st->betweenMrk and not SPLIT_AllowBetweenMarkers and m != markers.end()
				and m->second == (m->second > SAME ? 1 : -1) * st->mark_type.front()) {
			st->mark_type.pop_front();
			st->mark_form.pop_front();
			if (st->mark_type.empty()) {
				st->betweenMrk = false;
				st->no_split_count = 0;
			} else
				st->no_split_count++;
			st->buffer.push_back(*w);
			check_split = false;
		}
		// new marker being opened, push onto open markers stack,
		// we enter inbetween markers (or stay if we already were)
		else if (m != markers.end() and m->second > 0 and not SPLIT_AllowBetweenMarkers) {
			st->mark_form.push_front(m->first);
			st->mark_type.push_front(m->second);
			st->betweenMrk = true;
			st->no_split_count++;
			st->buffer.push_back(*w);
			check_split = false;
		}
		// regular word, inside markers. Do not split unless AllowBetweenMarkers is set.
		// Warn if we have been inside markers without splitting for very long.
		else if (st->betweenMrk) {
			st->no_split_count++;
			if (SPLIT_MaxWords == 0 or st->no_split_count <= SPLIT_MaxWords) {
				check_split = false;
				st->buffer.push_back(*w);
			}
			if (st->no_split_count == VERY_LONG) {
				if (st->no_split_count == VERY_LONG + 5) {
					cout << L"...etc..." << endl;
					cout << L"If this causes crashes try setting AllowBetweenMarkers=1 or" << endl;
					cout << L"setting a value other than 0 for MaxWords in your splitter" << endl;
					cout << L"configuration file (e.g. splitter.dat)" << endl;
				}
			}
		}
		// regular word outside markers, or AllowBtwenMarkers=1,
		// or MaxWords exceeded.  -->  Check for a sentence ending.
		if (check_split) {
			// check for possible sentence ending.
			map<wstring, bool>::const_iterator e = enders.find(*w);
			if (e != enders.end()) {
				// We split if the delimiter is "sure" (e->second==true) or if
				// context checking for sentence end returns true.
				if (e->second or end_of_sentence(w, v)) {
					// Complete the sentence
					st->buffer.push_back(*w);
					// store it in the results list
					ls.push_back(st->buffer);

					// Clear sentence, look for a new one
					st->buffer.clear();
					// reset state
					st->betweenMrk = false;
					st->no_split_count = 0;
					st->mark_type.clear();
					st->mark_form.clear();
				} else {
					// context indicated it was not a sentence ending.
					st->buffer.push_back(*w);
				}
			} else {
				// Normal word. Accumulate to the buffer.
				st->buffer.push_back(*w);
			}
		}
	}
// if flush is set, do not retain anything
	if (flush and not st->buffer.empty()) {
		// add sentence to return list
		ls.push_back(st->buffer);
		// reset state
		st->buffer.clear();
		st->no_split_count = 0;
		st->mark_type.clear();
		st->mark_form.clear();
		st->betweenMrk = false;
	}
}

///////////////////////////////////////////////////////////////
///  Sessionless splitting. Fill given list<sentece>
///////////////////////////////////////////////////////////////
void Splitter::split(const std::list<std::wstring> &lw, std::list<std::list<std::wstring>> &ls) const {
	session_id ss = open_session();
	split(ss, lw, true, ls);
	delete ss;
}

///////////////////////////////////////////////////////////////
///  Check whether a word is a sentence end (eg a dot followed
/// by a capitalized word).
///////////////////////////////////////////////////////////////
bool Splitter::end_of_sentence(list<wstring>::const_iterator w, const list<wstring> &v) const {
	list<wstring>::const_iterator r;
	wstring f;
// Search at given position w for the next word, to decide whether w contains a marker.
	if (w == --v.end()) {
		// Last word in the list. We don't know whether we can consider a sentence end.
		// Probably we'd better wait for next line, but we don't in this version.
		return true;
	} else {
		// Not last word, check the following word to see whether it is uppercase
		r = w;
		r++;
		f = *w;
		// See if next word a typical sentence starter or its first letter is uppercase
		return (RE_is_capitalized.search(f) || starters.find(f) != starters.end());
	}
}

