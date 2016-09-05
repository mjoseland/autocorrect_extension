autocorrectModule = null;  // Global application object.
statusText = 'NO-STATUS';

// a list of all current suggestions (empty when no suggestions are required)
//var suggestions = new Array();	

// a list of all text areas on the page
var textareaList = new Array();

// dictionary, maps text areas to previous value (string in area)
var textareaOldValues = new Array();

// most recently edited text area, not currently used but will be needed to place suggestions
// at the correct position in the page
var currentTextArea;

var keyUpTime;
var receivedMessageTime;

// sets the displayed suggestions on the test page
// suggestionsStr format: [word1] [word2] [word3] ...
function setSuggestions(suggestionsStr) {
	document.getElementById("suggestions").innerHTML = 'words: ' + suggestionsStr;
}

function setTimeTaken() {
	document.getElementById("time_taken").innerHTML = 'time: ' + 
		(receivedMessageTime - keyUpTime) + 'ms';
}

function clearSuggestions() {
	setSuggestions('-');
	document.getElementById("time_taken").innerHTML = 'time: -';

}


// TODO: ensure only one word has been changed, implement word_before_changed_word
// makes a new suggestion request message based on the new and old text in a textarea
// message format: rs:[changed_word] [word_before_changed_word]
// edge cases:
// 		no word before: send message "rs:[changed_word] -"
// 		change spans multiple words (eg. paste): no message sent, suggestions cleared
// 		change falsely reported (text area value is the same): no message sent
function makeSuggestionsRequestMessage(newText, oldText) {
	var i = newText.length;
	var j = oldText.length;

	// iterate backwards through each char. loop ends when i == 0, j == 0, or different chars
	// are located
	while (i-- && j-- && newText[i] == oldText[j]);

	if (newText[i] == ' ') {
		return '-';
	}

	var suggestionsRequestMessage = 'rs:';


	// find the start and end indexes of the modified word
	while (i < newText.length && ((newText[i] >= 'a' && newText[i] <= 'z') || 
				(newText[i] >= 'A' && newText[i] <= 'Z'))) {
		i++;
	}

	i--;
	var changedWordEnd = i;

	while (i >= 0 && ((newText[i] >= 'a' && newText[i] <= 'z') || 
				(newText[i] >= 'A' && newText[i] <= 'Z'))) {
		i--;
	}
	i++;

	var changedWordStart = i;

	suggestionsRequestMessage += newText.substring(changedWordStart, changedWordEnd + 1) + ' -';
	suggestionsRequestMessage = suggestionsRequestMessage.toLowerCase();

	console.log("makeSuggestionsRequestMessage() message generated: " 
			+ suggestionsRequestMessage);

	return suggestionsRequestMessage;
}


// event handler for a change in any text area, locates the change and requests new suggestions
// from the NaCL module if required
function handleTextAreaChange(changedTextareaIndex) {
	console.log('TA changed');	// this isn't firing tim
	currentTextarea = textareaList[changedTextareaIndex];
	var oldText = textareaOldValues[changedTextareaIndex];
	var newText = currentTextarea.value;
	textareaOldValues[changedTextareaIndex] = newText;

	suggestionsRequestMessage = makeSuggestionsRequestMessage(newText, oldText);

	// if a suggestions request is necessary, send one to the NaCL module
	if (suggestionsRequestMessage != '-') {
		autocorrectModule.postMessage(suggestionsRequestMessage);
	} else {
		clearSuggestions();
	}

}

function handleKeyUp() {
	// iterate through all original text areas on the page
	keyUpTime = new Date().getTime();
	for (var i = 0; i < textareaList.length; i++) {
		// if the text area is the active element
		if (textareaList[i] === document.activeElement) {
			// if the text area's value has changed, handle the change
			if (textareaList[i].value != textareaOldValues[i]) {
				console.log(typeof textareaList[i].value + ' ' + typeof textareaOldValues[i]);
				console.log(textareaList[i].value);
				console.log(textareaOldValues[i]);

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
	//var textFileUrl = 'chrome-extension:/__MSG_@@extension_id__/resources/top_50000_words.txt'
	var textFileUrl = chrome.extension.getURL('resources/top_50000_words.txt');
	//alert(textFileUrl);
	var httpRequest = new XMLHttpRequest();

    httpRequest.open('GET', textFileUrl, true);
    httpRequest.send(null);
    httpRequest.onreadystatechange = function () {
        if (httpRequest.readyState === 4 && httpRequest.status === 200) {
			//console.log(httpRequest.responseText.substring(0, 100));
			autocorrectModule.postMessage('wl:' + httpRequest.responseText);
        }
    }
}

// called on module load
function moduleDidLoad() {
  	autocorrectModule = document.getElementById('autocorrect_module');
	
	sendWordsToModule();


 	if (typeof jQuery == 'undefined') {
		console.log("JQ not loaded");
	} else {
		console.log("JQ loaded");
	} 

	// find all text areas and save current values to textareaOldValues
	textareaList = document.getElementsByTagName('textarea');

	for (var i = 0; i < textareaList.length; i++) {
		textareaOldValues.push(textareaList[i].value);
	}


	console.log(textareaList.length + ' text areas found');

	window.onkeyup = handleKeyUp;
}

// handles the receiving of a message from the NaCL module
// message types handled:
// 		new suggestions: ns:[word1] [word2] [word3]		(count of words may vary)
function handleMessage(messageEvent) {
	var receivedMessage = messageEvent.data;
	console.log('handleMessage() received: ' + receivedMessage);

	// set time taken element
	receivedMessageTime = new Date().getTime();
	setTimeTaken();

	
	if (receivedMessage.substring(0, 3) === 'ns:') {
		setSuggestions(receivedMessage.substring(3));
	}
}

// TODO: delete if not useful
// event handler for new page load, may want ot migrate textarea scanning to this function
function pageDidLoad() {
	console.log('Page loaded');
}

// TODO: delete if not useful
// updates the status of the NaCL module
function updateStatus(new_status) {
  	if (new_status)
		statusText = new_status;
}

