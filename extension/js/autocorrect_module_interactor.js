autocorrectModule = null;  // Global application object.
statusText = 'NO-STATUS';

// a list of all text areas on the page
var textareaList = new Array();

// dictionary, maps text areas to previous value (string in area)
var textareaOldValues = new Array();

// set (built from string) of all words that are considered to be part of a word
var inWordChars = {};
var inWordCharsStr = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\'-'

// most recently edited text area, not currently used but will be needed to place suggestions
// at the correct position in the page
var currentTextArea;

var moduleInitialised = false;
var wordResourceFile = 'resources/word_list.txt';

var onTestPage = false;

// sets the displayed suggestions on the test page
// 		suggestionsStr format: [word1] [word2] [word3] ...
// 		suggestionsStr = '-' will clear suggestions
function setSuggestions(suggestionsStr) {

	// if js is on a test page, update it
	if (onTestPage) {
		testPage.printSuggestions(suggestionsStr);

		if (suggestionsStr != '-') {
			testPage.printTimeTaken();
		} else {
			testPage.clearTimeTaken();
		}
	}
}

// TODO ensure only one word has been changed
// 		handle users pressing multiple keys quickly
//
// makes a new suggestion request message based on the new and old text in a textarea
// 		message format: rs:[changed_word] [word_before_changed_word]
// edge cases:
// 		no word, comma, or period before: send message "rs:[changed_word] -"
// 		change spans multiple words (eg. paste): no message sent, suggestions cleared
// 		change falsely reported (text area value is the same): no message sent
function makeSuggestionsRequestMessage(newText, oldText) {
	var i = newText.length;
	var j = oldText.length;

	// iterate backwards through each char. loop ends when i == 0, j == 0, or different chars
	// are located
	while (--i >= 0 && --j >= 0 && newText[i] == oldText[j]);

	if (!inWordChars.hasOwnProperty(newText[i])) {
		console.log('module interactor::makeSuggestionsRequestMessage(): last modified char ' +
				'is non-word char \'' + newText[i] + '\'');
		return '-';
	}

	// find the end of the changed word
	while (i < newText.length && inWordChars.hasOwnProperty(newText[i])) {
		i++;
	}
	i--;

	var changedWordEnd = i;

	// find the start of the changed word
	while (i >= 0 && inWordChars.hasOwnProperty(newText[i])) {
		i--;
	}
	i++;

	var changedWordStart = i;
	
	var changedWord = '-'

	if (changedWordStart <= changedWordEnd && 
			(changedWordStart == 0 || newText[changedWordStart - 1] == ' ')) {
		changedWord = newText.substring(changedWordStart, changedWordEnd + 1);
	}

	console.log(changedWordStart + ' ' + changedWordEnd);

	i--;

	// find the end of the previous word, if one exists
	while (i >= 0 && !inWordChars.hasOwnProperty(newText[i])) {
		i--;
	}

	var previousWordEnd = i;

	// find the start of the previous word, if one exists
	var previousWordStart = i;
	while (i >= 0 && inWordChars.hasOwnProperty(newText[i])) {
		i--;
	}
	i++;

	var previousWordStart = i;

	var previousWord = '-';

	// find the previous word, if there is one
	if (previousWordStart <= previousWordEnd && newText[previousWordEnd + 1] == ' ' &&
			(previousWordStart == 0 || newText[previousWordStart - 1] == ' ')) {
		previousWord = newText.substring(previousWordStart, previousWordEnd + 1);
	}

	// create suggestions request message
	var suggestionsRequestMessage = 'rs:' + changedWord + ' ' + previousWord;

	//suggestionsRequestMessage = suggestionsRequestMessage.toLowerCase();

	console.log('module interactor: makeSuggestionsRequestMessage() message generated: ' 
			+ suggestionsRequestMessage);

	return suggestionsRequestMessage;
}


// event handler for a change in any text area, locates the change and requests new suggestions
// from the NaCL module if required
function handleTextAreaChange(changedTextareaIndex) {
	console.log('module interactor: handleTextAreaChange(): textarea changed');

	var oldText = textareaOldValues[changedTextareaIndex];
	var newText = textareaList[changedTextareaIndex].value;
	textareaOldValues[changedTextareaIndex] = newText;

	suggestionsRequestMessage = makeSuggestionsRequestMessage(newText, oldText);

	// if a suggestions request is necessary, send one to the NaCL module
	if (suggestionsRequestMessage != '-') {
		autocorrectModule.postMessage(suggestionsRequestMessage);
	} else {
		setSuggestions('-');
	}

}

function handleKeyUp() {
	// if js is on test page, set its keyUpTime
	if (onTestPage) {
		testPage.setKeyUpTime(new Date().getTime());
	}

	// iterate through all original text areas on the page
	for (var i = 0; i < textareaList.length; i++) {
		// if the text area is the active element
		if (textareaList[i] === document.activeElement) {
			// if the text area's value has changed, handle the change
			if (textareaList[i].value != textareaOldValues[i]) {
				handleTextAreaChange(i);
			}

			break;
		}
	}
}

// TODO: find something cleaner, at least use pepper persistent storage after this has been
// called once.
//
// reads the words text file from the extension's directory and sends all words to the module
// in one message
function sendWordsToModule(){
	var textFileUrl = chrome.extension.getURL(wordResourceFile);
	var httpRequest = new XMLHttpRequest();

    httpRequest.open('GET', textFileUrl, true);
    httpRequest.send(null);
    httpRequest.onreadystatechange = function () {
        if (httpRequest.readyState === 4 && httpRequest.status === 200) {
			autocorrectModule.postMessage('wl:' + httpRequest.responseText);
        } else {
			console.log('module interactor: sendWordsToModule(): request or status not' +
																				' ready');
		}
    }
}

// called on module load
function moduleDidLoad() {
  	autocorrectModule = document.getElementById('autocorrect_module');

	if (testPage != undefined) {
		onTestPage = true;
	}

	// check if jQuery has been loaded
 	if (typeof jQuery == 'undefined') {
		console.log('module interactor: moduleDidLoad(): jQuery not loaded');
	} else {
		console.log('module interactor: moduleDidLoad(): jQuery loaded');
	} 

	// find all text areas and save current values to textareaOldValues
	textareaList = document.getElementsByTagName('textarea');

	for (var i = 0; i < textareaList.length; i++) {
		textareaOldValues.push(textareaList[i].value);
	}

	console.log('module interactor: moduleDidLoad(): ' + textareaList.length + 
																		' text areas found');

	// set handleKeyUp to be called ok keyup event
	window.onkeyup = handleKeyUp;

	// add all chars that are considered to be part of a word
	for (var i = 0; i < inWordCharsStr.length; i++) {
		inWordChars[inWordCharsStr[i]] = true;
	}

	// TODO, only call this after rw message received
	sendWordsToModule();
}

// handles the receiving of a message from the NaCL module
// message types handled:
// 		new suggestions: ns:[word1] [word2] [word3]		(count of words may vary)
function handleMessage(messageEvent) {
	var receivedMessage = messageEvent.data;
	console.log('module interactor: handleMessage() received: ' + receivedMessage);

	// if js is for a test page, set time taken element
	if (onTestPage) {
		testPage.setMessageReceivedTime(new Date().getTime());
	}

	// ns: new suggestions
	if (receivedMessage.substring(0, 2) === 'ns') {
		setSuggestions(receivedMessage.substring(3));
	}

	// rw: request words
	if (receivedMessage.substring(0, 2) === 'rw') {
		sendWordsToModule();
	}

	// si: module successfully initialised
	if (receivedMessage.substring(0, 2) === 'si') {
		moduleInitialised = true;
	}


}

// TODO: delete if not useful
//
// event handler for new page load, may want ot migrate textarea scanning to this function
function pageDidLoad() {
	console.log('module interactor: pageDidLoad(): page loaded');
}

// TODO: delete if not useful
//
// updates the status of the NaCL module
//function updateStatus(new_status) {
  	//if (new_status)
		//statusText = new_status;
//}

