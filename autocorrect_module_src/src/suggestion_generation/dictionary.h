//
// Created by Malcolm Joseland on 23/07/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_DICTIONARY_H
#define GLOBAL_AUTOCOMPLETE_DICTIONARY_H

#define CLOSEST_WORDS_N 5   // the max number of words to add to _closest_words
#define MAX_ED 15            // the max ed for a word to be added to _closest_words
#define PSPEECH_COUNT 24
#define UNCL_PSPEECH_INDEX PSPEECH_COUNT - 1

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

#include "word.h"

using namespace std;

// A dictionary of words. Generates the best suggestions for an input word.
// The input word is added one character at a time through the addCompareChar() method
// The best suggestions are retrieved using the getClosestWords() method
//
// Each word beongs to one or more parts of speech
// 		eg. a verb is identified with pspeech_str "Verb", pspeech = 0
// 		pspeech_str is the string ID of a part of speech, used by the file that stores all
// 			words
// 		pspeech is the identifying integer of a part of speech
//
// Parts of speech	(* = no words currently)
// pspeech str  	 description
//		0  Verb      verb, default? (jump, study)
//		1  VerbTPP   verb, third-person present (sails)
//		2  VerbPa    verb, past tense (incl. participle) (sailed, had, rode, ridden)
//		3  VerbGe    verb/gerund, research on usage rules (sailing)
//		4  NoC       common noun (island, ship)
//		5  NoCPl     common noun, plural (islands, dogs)
//		6  NoCPo     *common noun, possessive (pig's)
//		7  NoP       proper noun (London)
//		8  NoPPl     proper noun, plural (Andersons)
//		9  NoPPo     proper noun, possessive (London's)		- added manually
//		10 Adv       adverb, follows or precedes a verb (angrily, in)
//		11 Adj       adjective, follows or precedes a noun (willing, small)
//		12 VMod		 verb modifier/helper (should, might, could)
//		13 DetP      (enough, latter, what)
//		14 Prep      preposition (beyond, according, in)
//		15 Pron      pronoun (she, hers, someone, they)
//		16 Conj      conjunction (and, but, if)
//		17 Int       interjection (yikes, uh-oh)
//		18 Fore      *???									- possibly delete, not common
//		19 Det		 your, every, the
//		20 Num       number (zero)
//		21 Inf		 single word : to 
//		22 Neg		 single word : not
//		23 Uncl      *unclassified
class Dictionary{
public:
    Dictionary();

    vector<string> getClosestWords();

    void addWord(const string &word, const string &pspeech_str, const int &count, 
			const float &frequency);
    void addCompareChar(char c, string &previous_word);

	// resets all scoring for word suggestions, ie. gets ready to start providing suggestions 
	// for a new word
    void resetCompareWord();

	// finalise the dictionary : becomes usable but no new words can be added
	void finalise();

	bool ready();

private:
	bool finalised_ = false;
	string previous_word_ = "-";

	// vector of all "pos vectors" in the dictionary
	// each pos vector contains the words from a specific part of speech
    vector<vector<Word>> words_;

	// maps words to their part of speech index
	// ie. "available" : 11
	unordered_multimap<string, uint8_t> word_pspeech_map_;

	// maps part of speech strings to their index 
	// ie. pspeech_str : pspeech, "Verb" : 0
	unordered_map<string, uint8_t> pspeech_map_;
  
	// 
	vector<vector<bool>> pspeech_order_vector_;
	
	vector<bool> correct_pspeech_vector_;

	// indexes of all closest words in descending order of suitability
	// first : pos vector index | second : word index
    list<pair<size_t, size_t>> closest_word_indexes_;

    void insertNewClosestWordIndex(size_t i, size_t j);

	// generates a map of parts of speech : index in word_lists_
	// ie. "Verb" : 0
	void constructPspeechMap();
	void constructPspeechOrderVector();
	void updatePspeechPenalties();


	uint8_t get_pspeech(string word);
	// returns true if a word is currently in the list of closest words in any form.
	// 		return true if word = "jump" and "jump" is already in the list
	bool in_closest_words(string word_str);
};


#endif //GLOBAL_AUTOCOMPLETE_DICTIONARY_H


