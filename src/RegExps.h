/*
 * RegexP.h
 *
 *  Created on: 2017-10-31
 *      Author: litaoxiao
 */

#ifndef SRC_REGEXPS_H_
#define SRC_REGEXPS_H_
#include <boost/regex.hpp>
#include <string>
#include <vector>

class RegExps {

private:
	typedef boost::wregex regex_type;
	typedef boost::wsmatch match_type;
	// internal regular expression
	regex_type re;

	// private function: convert internal match list to vector<string>
	void extract_matches(const match_type &, std::vector<std::wstring> &) const;
	// private function: convert internal match list to vector<string> and positions to vector<int>
	void extract_matches(const match_type &, std::vector<std::wstring> &, std::vector<int> &) const;

public:
	RegExps(const std::wstring &expr);
	~RegExps();
	/// Search for a partial match in a string
	bool search(const std::wstring &in, bool continuous = false) const;
	/// Search for a partial match in a string, return sub matches
	bool search(const std::wstring &in, std::vector<std::wstring> &out, bool continuous = false) const;
	/// Search for a partial match in a string, return sub matches and positions
	bool search(const std::wstring &in, std::vector<std::wstring> &out, std::vector<int> &pos,
			bool continuous = false) const;
	/// Search for a partial match in a string, return sub matches
	bool search(std::wstring::const_iterator i1, std::wstring::const_iterator i2, std::vector<std::wstring> &out,
			bool continuous = false) const;
	/// Search for a partial match in a string, return sub matches and positions
	bool search(std::wstring::const_iterator i1, std::wstring::const_iterator i2, std::vector<std::wstring> &out,
			std::vector<int> &pos, bool continuous = false) const;
	/// Search for a whole match in a string
	bool match(const std::wstring &in) const;
	/// Search for a whole match in a string, return sub matches
	bool match(const std::wstring &in, std::vector<std::wstring> &out) const;
};
#endif /* SRC_REGEXPS_H_ */
