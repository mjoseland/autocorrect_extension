//
// Created by Malcolm Joseland on 23/07/2016.
//


#ifndef GLOBAL_AUTOCOMPLETE_WORD_H
#define GLOBAL_AUTOCOMPLETE_WORD_H

#define MIN(a, b) ((a < b) ? a : b)
#define FIRST_LETTER_PENALTY_MULT 3
#define LEN_MISMATCH_PENALTY_MULT 0
#define INCORRECT_PSPEECH_PENALTY 7

#include <vector>
#include <string>
#include <iostream>

#include "edit_cost_array.h"
#include "ed_matrix.h"

#ifndef COST
#define COST uint8_t
#endif

using namespace std;

class Word{
public:
    Word(const string &word, const uint32_t &count, const float &distribution);

    string getWord();
    size_t getLength();
	// get the cost of replacing the comare word with this word
    COST getCost();
	// get the frequency count of the word
    uint32_t getCount();

	// add another char to the compare word
    void addCompareChar(char c);
	// set whether or not this word is the correct part of speech
	void setCorrectPspeech(bool is_correct_pspeech);
	// reset the compare word
    void resetCompareWord();

    // Return true if this word is a less suitable match than the word it is being compared 
	// to
    bool operator<(Word other_word);
    // Return true if this word's suitability is equal to or less than the other word
    bool operator<=(Word other_word);
    // Return true if this word is a more suitable match than the other word
    bool operator>(Word other_word);
    // Return true if this word's suitability is equal to or more than the other word
    bool operator>=(Word other_word);
    // Return true if this word has an equal match suitability to the other word
    bool operator==(Word other_word);


private:
    const string word_;			// TODO custom data type to save memory, SmallString?

    const uint32_t count_;  	// the frequency count of the word (larger value : word used
								// more often) (TODO: change to uint16_t to optimise memory)
	const float distribution_;	// the distribution of the word : higher dist used in a wider
								// range of contexts

    uint8_t j_;             	// the "index" of the next char to be added in the matrix
                            	// (if it wasn't two columns) (TODO: change to uint8_t)
							
	char previous_char_ = '*';	// the char that was previously added to the compare word

	COST cost_;					// cost of replacing the compare word with this word
	bool is_correct_pspeech_;	// true if this word is the correct part of speech as a
								// suggestion
	EdMatrix ed_matrix_;

    uint8_t get_element_ed(char c, size_t i);
    uint8_t min_of_three(uint8_t a, uint8_t b, uint8_t c);
    int compare_suitability(Word other_word);   // TODO inline?
};

#endif //GLOBAL_AUTOCOMPLETE_WORD_H

