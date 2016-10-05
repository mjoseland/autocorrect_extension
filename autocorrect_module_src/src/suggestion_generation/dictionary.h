//
// Created by Malcolm Joseland on 23/07/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_WORDLIST_H
#define GLOBAL_AUTOCOMPLETE_WORDLIST_H

#define CLOSEST_WORDS_N 5   // the max number of words to add to _closest_words
#define MAX_ED 15            // the max ed for a word to be added to _closest_words
#define POS_COUNT 24

#include <vector>
#include <list>
#include <unordered_map>

#include "word.h"

using namespace std;

class Dictionary{
public:
    Dictionary();

    vector<string> getClosestWords();

    void addWord(string word, string pos_str, int count, float frequency);
    void addCompareChar(char c);

	// resets all scoring for word suggestions, ie. gets ready to start providing suggestions 
	// for a new word
    void resetCompareWord();

	// finalise the dictionary : becomes usable but no new words can be added
	void finalise();

private:
	bool finalised_ = false;

	// vector of all "pos vectors" in the dictionary
	// each pos vector contains the words from a specific part of speech
    vector<vector<Word>> words_;

	// maps words to their part of speech index
	// ie. "available" : 11
	unordered_map<string, size_t> word_pos_index_map_;

	// maps part of speech strings to their index
	// ie. "Verb" : 0
	unordered_map<string, size_t> pos_str_index_map_;
  
	// indexes of all closest words in descending order of suitability
	// first : pos vector index | second : word index
    list<pair<size_t, size_t>> closest_word_indexes_;

    void insertNewClosestWordIndex(size_t i, size_t j);

	// generates a map of parts of speech : index in word_lists_
	// ie. "Verb" : 0
	unordered_map<string, size_t> getPosIndexMap();
};


#endif //GLOBAL_AUTOCOMPLETE_WORDLIST_H

// Parts of speech
//		0  Verb      verb, default? (sail)
//		1  VerbUs    verb, only after third-person singular noun (sails) 	*Us = unnamed "s"
//		2  VerbPa    verb, past tense (sailed, written, withdrawn)
//		3  VerbGe    verb/gerund, research on usage rules (sailing)
//		4  NoC       common noun
//		5  NoCPo     common noun, possessive (pig's) 		*not currently included
//		6  NoCPl     common noun, plural
//		7  NoP       proper noun (London)
//		8  NoPPl     proper noun, plural (Andersons)
//		9  NoPPo     proper noun, possessive (London's)	*added manually
//		10 Adv       adverb, follows or precedes a verb (angrily)
//		11 Adj       adjective, follows or precedes a noun
//		12 VMod		 verb modifier/helper (should, might, could)
//		13 DetP      (enough, latter)
//		14 Prep      preposition (?)
//		15 Pron      pronoun (she, it)
//		16 Conj      and, but
//		17 Int       interjection? 
//		18 Fore      ???
//		19 Det		 your, hers, every
//		20 Num       number (zero)
//		21 Inf		 single word : to 
//		22 Neg		 single word : not
//		23 Uncl      unclassified							*not currently included

