//
// Created by Malcolm Joseland on 3/08/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_AUTOCORRECTINSTANCE_H
#define GLOBAL_AUTOCOMPLETE_AUTOCORRECTINSTANCE_H

#include <iostream>

#include "ppapi/cpp/instance.h"
//#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
//#include "ppapi/cpp/instance_handle.h"
//#include "ppapi/utility/threading/simple_thread.h"
//#include "ppapi/cpp/file_system.h"

#include "../suggestion_generation/suggestion_generator.h"
//#include "persistent_storage_io.h"

using namespace std;

class AutocorrectInstance : public pp::Instance {
public:
	virtual ~AutocorrectInstance() {
		//delete persistent_storage_io_;
		delete s_instance_; 
	}

	static void createInstance(PP_Instance pp_instance) {
		if (s_instance_ == nullptr) {
			s_instance_ = new AutocorrectInstance(pp_instance);
		}
	}

	static AutocorrectInstance *instance() {
		if (s_instance_ == nullptr) {
			cerr << "AutocorrectInstance: getInstance() returninng nullptr\n";
		}

		return s_instance_;
	}

	void HandleMessage(const pp::Var& message);

private:
	SuggestionGenerator suggestion_generator_;

	static AutocorrectInstance *s_instance_;

	//PersistentStorageIo *persistent_storage_io_;


	explicit AutocorrectInstance(PP_Instance instance);

	static void postString(string message_str) {
		if (s_instance_ == nullptr) {
			cerr << "AutocorrectInstance: PostMessage(string) called with no static "
				"instance\n";
			
			return;
		}

		cout << "AutocorrectInstance::postString(): Posting message \"" << message_str <<
			"\" to browser\n\n";

		pp::Var message_var = pp::Var(message_str);

		s_instance_->PostMessage(message_var);
	}
};


#endif //GLOBAL_AUTOCOMPLETE_AUTOCORRECTINSTANCE_H
