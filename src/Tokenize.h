/*
 * Tokenize.h
 *
 *  Created on: 2017-10-31
 *      Author: litaoxiao
 */

#ifndef SRC_TOKENIZE_H_
#define SRC_TOKENIZE_H_

#include <set>
#include <map>
#include <list>
#include <string>
#include "RegExps.h"

class Tokenize {
public:
	/// Constructor
	Tokenize();
	virtual ~Tokenize();
	static void init_tokenize(const std::string&);
	/// tokenize string, leave result in lw
	void tokenize(const std::wstring &text, std::list<std::wstring> &lw) const;
private:
	/// abreviations set (Dr. Mrs. etc. period is not separated)
	static std::set<std::wstring> abrevs;
	/// tokenization rules
	static std::list<std::pair<std::wstring, RegExps> > rules;
	/// substrings to convert into tokens in each rule
	static std::map<std::wstring, int> matches;
};

#endif /* SRC_TOKENIZE_H_ */
