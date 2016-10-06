//
// Created by Malcolm Joseland on 23/07/2016.
//


#ifndef GLOBAL_AUTOCOMPLETE_WORD_H
#define GLOBAL_AUTOCOMPLETE_WORD_H

#define MIN(a, b) ((a < b) ? a : b)
#define FIRST_LETTER_PENALTY 3

#include <vector>
#include <string>
#include <iostream>

#include "edit_cost_array.h"

using namespace std;

class Word{
public:
    Word(string word, uint32_t count, float distribution);

    string getWord();
    size_t getLength();
    uint8_t getCost();
    uint32_t getCount();

	// add another char to the compare word
    void addCompareChar(char c);
	// reset the compare word
    void resetCompareWord();

    // Return true if this word is a less suitable match than the word it is being compared 
	// to
    bool operator<(Word compareword_);
    // Return true if this word's suitability is equal to or less than compareword_
    bool operator<=(Word other_word);
    // Return true if this word is a more suitable match than the other word
    bool operator>(Word other_word);
    // Return true if this word's suitability is equal to or more than the other word
    bool operator>=(Word other_word);
    // Return true if this word has an equal match suitability to the other word
    bool operator==(Word other_word);


private:
    const string word_;
    const size_t word_len_;
    const uint32_t count_;  // the frequency count of the word (larger value : word used more 
							// often)
	const float distribution_;	// the distribution of the word : higher = used in a wider
								// variety of situations

    size_t j_;              // the "index" of the next char to be added in the matrix
                            // (if it wasn't two columns)
	char previous_char_ = '*';	// the char that was previously added to the compare word
    uint8_t min_ed_;
    bool current_column_;   // index to calculate ed of next char added in ed_matrix_
    vector<vector<uint8_t>> ed_matrix_; // 2-colummn matrix for calculating edit distance

    uint8_t get_element_ed(char c, size_t i);
    uint8_t min_of_three(uint8_t a, uint8_t b, uint8_t c);
    int compare_suitability(Word other_word);   // TODO inline?
};

#endif //GLOBAL_AUTOCOMPLETE_WORD_H

