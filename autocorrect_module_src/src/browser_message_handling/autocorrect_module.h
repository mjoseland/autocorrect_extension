//
// Created by Malcolm Joseland on 3/08/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_AUTOCORRECTMODULE_H
#define GLOBAL_AUTOCOMPLETE_AUTOCORRECTMODULE_H

#include "ppapi/cpp/module.h"
#include "ppapi/cpp/instance.h"

//#include <iostream>

#include "autocorrect_instance.h"

class AutocorrectModule : public pp::Module {
	public:
		AutocorrectModule() : pp::Module() {}

		virtual ~AutocorrectModule() {}

		virtual pp::Instance *CreateInstance(PP_Instance pp_instance) {
			AutocorrectInstance::createInstance(pp_instance);

			AutocorrectInstance *instance = AutocorrectInstance::instance();

			return instance;
		}
};

namespace pp {
	Module *CreateModule() {
		return new AutocorrectModule();
	}
}

AutocorrectInstance *AutocorrectInstance::s_instance_ = nullptr;

#endif // GLOBAL_AUTOCOMPLETE_AUTOCORRECTMODULE_H
