/*
 * Main.cc
 *
 *  Created on: 2017-11-01
 *      Author: litaoxiao
 */

#include "Splitter.h"
#include "Tokenize.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	Tokenize::init_tokenize("/home/timxiao/Program/svn/nlp_proj/branches/EN_Seg/tokenizer.dat");
	Splitter::init_splitter("/home/timxiao/Program/svn/nlp_proj/branches/EN_Seg/splitter.dat");
	Tokenize tokenize;
	Splitter splitter;
	list<wstring> lw;
	tokenize.tokenize(L"Hello! My name is litaoxiao. This number is 2.4. What do you think? I love this game (chicken run run)! Yes,",
			lw);
	wcout << lw.size() << endl;
	list<std::list<std::wstring>> ls;
	splitter.split(lw, ls);
	for(auto sentence: ls){
		for(auto word: sentence){
			wcout << word << "#";
		}
		wcout << endl;
	}
}
