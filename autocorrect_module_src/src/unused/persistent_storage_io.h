// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


//
// Created by Malcolm Joseland on 3/11/2016.
//

#ifndef GLOBAL_AUTOCOMPLETE_PERSISTENTSTROAGEIO_H
#define GLOBAL_AUTOCOMPLETE_PERSISTENTSTROAGEIO_H

#define __STDC_LIMIT_MACROS

#include <iostream>
#include <string>
#include <vector>

#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/ppb_file_io.h"
#include "ppapi/cpp/directory_entry.h"
#include "ppapi/cpp/file_io.h"
#include "ppapi/cpp/file_ref.h"
#include "ppapi/cpp/file_system.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/message_loop.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_array.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/utility/threading/simple_thread.h"

#ifndef INT32_MAX
#define INT32_MAX (0x7FFFFFFF)
#endif

using namespace std;

class PersistentStorageIo {
public:
	PersistentStorageIo(pp::SimpleThread *io_thread, pp::FileSystem *file_system);
			//pp::InstanceHandle &instance_handle);

	virtual ~PersistentStorageIo() {
		io_thread_->Join();
	}

	// returns the version number of the word list in persistent storage
	// if no word list exists: return -1
	int getWordListVersion();

	// writes a word list to persistent storage
	// if a word list already exists, this one will replace it
	void storeWordList(string &wordListStr);


private:
	bool ready_ = false;
	//pp::Instance module_instance_;

	// thread used for io operations
	pp::SimpleThread *io_thread_;
	pp::FileSystem *file_system_;

	pp::CompletionCallbackFactory<PersistentStorageIo> callback_factory_;

	void openFileSystem();

	void getFileList(const string &dir_name, vector<string> &file_list);

	void ListCallback(int32_t result, 
		const vector<pp::DirectoryEntry> &entries, pp::FileRef);//vector<string> &file_list);
};

#endif // GLOBAL_AUTOCOMPLETE_PERSISTENTSTROAGEIO_H

