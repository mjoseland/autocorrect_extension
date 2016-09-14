//
// Created by Malcolm Joseland on 14/09/2016.
//

#include <array>

#include "definitions.h"

// costs of different edit operations
// insert/delete
#define IDN 2	// non-letter char, ie. one char is a "-" or "'"
#define IDS 3	// char is same as previous or previous = * (no previous)
#define IDD 4	// default

// substitution
#define KP 3	// letter is keyboard-proximal (1 button away)
//#define SP 6	// letter is semi-proximal (proximal to a proximal) (~2 buttons away)
#define NP 8	// letter isn't proximal (>2 buttons away)
#define NC 10	// either char is a non-letter char, ie. "-" or "'"

using namespace std;

// TODO: possibly make all constexpr, not doing it now cause it's too much trobule without c++14
class EditCostArray {
	public:

		static COST insDelCost(char previous_char, char new_char, char word_char);
		static COST subCost(char new_char, char word_char);


	private:

		static const COST *substitution_costs_;

};
