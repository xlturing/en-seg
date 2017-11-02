/*
 * Splitter.h
 *
 *  Created on: 2017-11-01
 *      Author: litaoxiao
 */

#ifndef SRC_SPLITTER_H_
#define SRC_SPLITTER_H_

#include <map>
#include <set>
#include <list>
#include <string>
#include "RegExps.h"

class Splitter {
private:
/// configuration options
	static bool SPLIT_AllowBetweenMarkers;
	static int SPLIT_MaxWords;
/// Sentence delimiters
	static std::set<std::wstring> starters;
	static std::map<std::wstring, bool> enders;
/// Open-close marker pairs (parenthesis, etc)
	static std::map<std::wstring, int> markers;
	static RegExps RE_is_capitalized;

/// check for sentence markers
	bool end_of_sentence(std::list<std::wstring>::const_iterator, const std::list<std::wstring> &) const;

// current state: Remember half-recognized sentences and open markers.
	class splitter_status {
	public:
		bool betweenMrk;
		int no_split_count;
		std::list<int> mark_type;
		std::list<std::wstring> mark_form;
		/// words of current sentence accumulated so far.
		std::list<std::wstring> buffer;
		/// count sentences to assign them id numbers
		size_t nsentence;
	};

public:
	/// Constructor, given option file
	Splitter();
	/// Destructor
	~Splitter();
	static void init_splitter(const std::string&);

	typedef splitter_status* session_id;
	/// open a splitting session, get session id
	session_id open_session() const;
	/// close splitting session
	void close_session(session_id ses) const;

	/// Add given list of words to the buffer, and put complete sentences
	/// that can be build into ls.
	/// The boolean states if a buffer flush has to be forced (true) or
	/// some words may remain in the buffer (false) if the splitter
	/// needs to wait to see what is coming next.
	/// Each thread using the same splitter needs to open a new session.
	void split(session_id ses, const std::list<std::wstring> &lw, bool flush, std::list<std::list<std::wstring>> &ls) const;

	// sessionless splitting.
	// Equivalent to opening a session, split with flush=true, and close the session
	void split(const std::list<std::wstring> &lw, std::list<std::list<std::wstring>> &ls) const;
};

#endif /* SRC_SPLITTER_H_ */
