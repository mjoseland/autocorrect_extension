//
// Created by Malcolm Joseland on 14/09/2016.
//
//
#ifndef GLOBAL_AUTOCOMPLETE_EDITCOSTARRAY_H
#define GLOBAL_AUTOCOMPLETE_EDITCOSTARRAY_H

#include <array>

#include <vector>
#include <array>
#include <string>

#ifndef COST
#define COST uint8_t
#endif

// costs of different edit operations
// insert/delete
#define IDN 2	// non-letter char, ie. one char is a "-" or "'"
#define IDS 3	// char is same as previous or previous = * (no previous)
#define IDD 4	// default

// substitution
#define KP 3	// letter is keyboard-proximal (1 button away)
//#define SP 6	// letter is semi-proximal (proximal to a proximal) (~2 buttons away)
#define NP 8	// letter isn't proximal (>=2 buttons away)
#define NC 10	// either char is a non-letter char, ie. "-" or "'"

using namespace std;

// TODO: possibly make everythingconstexpr, not doing it now cause it's too much trouble 
// without c++14
class EditCostArray {
public:
	// class to initialise substitution_costs_
	class SubstitutionCostInitialiser {
	public:
		SubstitutionCostInitialiser();
	};

    friend class Initializer;
		
	// the cost of substituting one char for another
	static vector<COST> substitution_costs_;

	// initialise class to initialise the variable above
	static SubstitutionCostInitialiser initialiser;

	static COST insDelCost(char previous_char, char new_char, char word_char);
	static COST subCost(char new_char, char word_char);
private:
};

#endif // GLOBAL_AUTOCOMPLETE_EDITCOSTARRAY_H
