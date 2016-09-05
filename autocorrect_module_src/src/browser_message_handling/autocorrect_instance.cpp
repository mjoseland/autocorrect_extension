//
// Created by Malcolm Joseland on 3/08/2016.
//

//#include <fstream>
//#include <iostream>
//#include <cstdlib>

#include "autocorrect_instance.h"


void AutocorrectInstance::HandleMessage(const pp::Var &message) {
	string message_str = message.AsString();

	if (message_str.substr(0, 3) != "wl:") {
		cout << "AutocorrectInstance::HandleMessage: Received message: \"" << message_str << 
			"\" from browser\n";
	}

	// handle suggestions request message
	if (message_str.substr(0, 3) == "rs:") {
		size_t space_i;
		for (space_i = 2; space_i < message_str.size() && message_str[space_i] != ' '; space_i++);

		string suggestions = suggestion_generator_.getSuggestions(
				message_str.substr(3, space_i - 3), 
				message_str.substr(space_i + 1));

		// send new suggestions back to the browser
		postString("ns:" + suggestions);
		return;
	}

	// handle the browser sending the word list to use
	if (message_str.substr(0, 3) == "wl:" && !words_added_) {
		words_added_ = true;
		suggestion_generator_.useWords(message_str.substr(3));
	}
}


/* -------------------- private functions -------------------- */

