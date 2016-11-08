// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//
// Created by Malcolm Joseland on 3/11/2016.
//

#include "persistent_storage_io.h"

PersistentStorageIo::PersistentStorageIo(pp::SimpleThread *io_thread, 
		pp::FileSystem *file_system) : 
		//pp::InstanceHandle &instance_handle) :
		ready_(false), 
		//module_instance_(instance_handle), 
		io_thread_(io_thread),//pp::InstanceHandle(&autocorrect_instance)),
		file_system_(file_system),
		callback_factory_(this) {
	// start thread used for io operations
	io_thread_->Start();

	// open/initalise the file system
	io_thread_->message_loop().PostWork(callback_factory_.NewCallback(
				&PersistentStorageIo::openFileSystem));

	vector<string> test_vector;
	getFileList("./", test_vector);
}

/* -------------------- private functions -------------------- */

void PersistentStorageIo::openFileSystem() {
	int32_t rv = file_system_->Open(1024 * 1024, pp::BlockUntilComplete());
	if (rv == PP_OK) {
		ready_ = true;
		cout << "PersistentStorageIo::OpenFileSystem(): opened file system" << endl;
	} else {
		cerr << "PersistentStorageIo::OpenFileSystem(): failed to open file system\n";
	}
}

void PersistentStorageIo::getFileList(const string &dir_name, vector<string> &file_list) {
	if (!ready_) {
		cerr << "PersistentStorageIo::listFiles(): file system not ready" << endl;
		return;
	}

	pp::FileRef ref(*file_system_, dir_name.c_str());
	ref.ReadDirectoryEntries(callback_factory_.NewCallbackWithOutput(
				&PersistentStorageIo::ListCallback, ref));

	//file_list = ref.file_list;

	//for (auto &str: file_list) {
		//cout << file_list << endl;
	//}

	// return file list
}


void PersistentStorageIo::ListCallback(int32_t result, //vector<string> file_list, pp::FileRef)
		const vector<pp::DirectoryEntry> &entries, pp::FileRef) {//vector<string> &file_list) {
	if (result != PP_OK) {
		//ShowErrorMessage("List failed", result);
		return;
	}
	
	// use file list instead
	vector<string> sv;

	for (size_t i = 0; i < entries.size(); ++i) {
    	pp::Var name = entries[i].file_ref().GetName();
    	if (name.is_string()) {
    		sv.push_back(name.AsString());
			cout << name.AsString() << endl;
       	}
	}

	cout << "GH\n";	//TODO del

	//PostArrayMessage("LIST", sv);
	//ShowStatusMessage("List success");
}

