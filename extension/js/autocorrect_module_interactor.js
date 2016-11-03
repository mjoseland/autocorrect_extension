autocorrectModule = null;  // autocorrect native client module
statusText = 'NO-STATUS';

// a list of all text areas on the page
var inputElements = new Array();

// set (built from string) of all words that are considered to be part of a word
var inWordChars = {};
var inWordCharsStr = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\'-'

// most recently edited text area, not currently used but will be needed to place suggestions
// at the correct position in the page
var currentTextArea;

var moduleInitialised = false;
var wordResourceFile = 'resources/word_list.txt';

//var getCaretCoordinates = require('textarea-caret');
var coordinates;

// TODO console.log only if onTestPage is true
// true if suggestions are being generated for the test page
var onTestPage = false;

// removes all autocorrect_suggestions elements
function removeSuggestionsElements() {
	var suggestionsElements = document.getElementsByClassName("autocorrect_suggestions");

	for (var i = 0; i < suggestionsElements.length; i++) {
		suggestionsElements[i].parentElement.removeChild(suggestionsElements[i]);
	}
}

function updateCoordinates(inputElementIndex) {
	var baseElement = inputElements[inputElementIndex].getBaseElement();

	coordinates = getCaretCoordinates(baseElement, baseElement.selectionEnd);

	var fontSize = parseFloat(
			window.getComputedStyle(baseElement, null).getPropertyValue('font-size'));

	fontSize += fontSize / 3;

	coordinates.top += baseElement.offsetTop - baseElement.scrollTop + fontSize;
	coordinates.left += baseElement.offsetLeft - baseElement.scrollLeft;

	console.log('updateCareCoordinates(): ' + coordinates.top + ' ' + coordinates.left);
}

function createSuggestionsElement(suggestionsStr) {
	// remove all autocorrect_suggestions elements
	removeSuggestionsElements();

	if (coordinates == null) {
		updateCoordinates();
	}

	//console.log('TEST: ' + coordinates.top + ' ' + coordinates.left);

	// create new autocorrect_suggestions element
    var elementStr = '<div class="autocorrect_suggestions"><table>\n';

	var suggestedWords = suggestionsStr.split(" ");
	for (var i = 0; i < suggestedWords.length - 1; i++) {
		elementStr += '<tr><td>' + suggestedWords[i] + '</td></tr>\n';
	}
	elementStr += '<tr><td>' + suggestedWords[suggestedWords.length - 1] + '</td></tr>\n';

	elementStr += '</table></div>\n';

    var suggestionsElement = document.createElement('div');
    suggestionsElement.innerHTML = elementStr;

	console.log(elementStr);

	// add element to document
   	document.body.appendChild(suggestionsElement.firstChild);

	// set offset of new element (for some reason this needs to be done after adding
	suggestionsElements = document.getElementsByClassName("autocorrect_suggestions");
	suggestionsElements[0].style.top = coordinates.top + "px";
	suggestionsElements[0].style.left = coordinates.left + "px";
}

// sets the displayed suggestions on the test page
// 		suggestionsStr format: [word1] [word2] [word3] ...
// 		suggestionsStr = '-' will clear suggestions
function setSuggestions(suggestionsStr) {
	// if js is on a test page, update it
	if (onTestPage) {
		testPage.printSuggestions(suggestionsStr);

		if (suggestionsStr == '-') {
			testPage.clearTimeTaken();
		} else {
			testPage.printTimeTaken();
		}
	}

	if (suggestionsStr == '-') {
		removeSuggestionsElements();
	} else {
		createSuggestionsElement(suggestionsStr);
	}

}

// TODO ensure only one word has been changed
// 		handle users pressing multiple keys quickly
// 		handle new lines correctly
// 		hash previously retrieved suggestions (limit size/bloat)
// 		typing symbol+char results in a small box (no suggestions should be none displayed)
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
		console.log('makeSuggestionsRequestMessage(): last modified char is non-char \'' +
				newText[i] + '\'');
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

	console.log('makeSuggestionsRequestMessage() message generated: ' +
			suggestionsRequestMessage);

	return suggestionsRequestMessage;
}


// event handler for a change in any text area, locates the change and requests new 
// suggestions from the NaCL module if required
// 		inputElementIndex 	the index of the input element that was changed
function handleTextAreaChange(inputElementIndex) {
	// if js is on test page, set its keyUpTime
	if (onTestPage) {
		testPage.setKeyUpTime(new Date().getTime());
	}

	console.log('handleTextAreaChange(): textarea changed');

	var oldText = inputElements[inputElementIndex].getOldValue();
	var newText = inputElements[inputElementIndex].getValue();
	inputElements[inputElementIndex].setOldValueToCurrent();

	suggestionsRequestMessage = makeSuggestionsRequestMessage(newText, oldText);

	// if a suggestions request is necessary, send one to the NaCL module
	if (suggestionsRequestMessage != '-') {
		autocorrectModule.postMessage(suggestionsRequestMessage);
	} else {
		console.log("GH");
		setSuggestions('-');
		updateCoordinates(inputElementIndex);
	}

}

function handleInputEvent() {
	//var coordinates = getCaretCoordinates(this, this.selectionEnd);
	//
	//console.log(inputElements.length);

	//console.log('handleTextAreaChange(): coordinates: ' + coordinates.top + 
			//' ' + coordinates.left);

	// iterate through all original text areas on the page
	for (var i = 0; i < inputElements.length; i++) {
		// if the text area is the active element
		if (inputElements[i].isActiveElement()) {
			if (inputElements[i].getValue() == '') {
				updateCoordinates(i);
				break;
			}
			
			// if the text area's value has changed, handle the change
			if (inputElements[i].hasUnhandledChange()) {
			//if (this.hasUnhandledChange()) {
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
			console.log('sendWordsToModule(): request or status not ready');
		}
    }
}

// safe wrapper for HTMLTextAreaElement or HTMLInputElement
function TextInputElement(baseElement) {
	this.getValue = function() {
		return baseElement.value;
	}

	this.addEventListener = function(event, handler) {
		element.addEventListener(event, handler);
	}

	this.isActiveElement = function(event, handler) {
		return (element === document.activeElement);
	}

	this.setOldValueToCurrent = function() {
		oldValue = this.getValue();
	}

	this.getOldValue = function() {
		return oldValue;
	}

	this.hasUnhandledChange = function() {
		return !(oldValue === element.value);
	}

	this.getBaseElement = function() {
		return baseElement;
	}

	var element = baseElement;
	var oldValue = this.getValue();
}

function addCss() {
	// 		     font	 		     fill			 	 hover-fill			 border
	// grey
	var grey  = ['rgb(015,015,015)', 'rgb(230,230,230)', 'rgb(250,250,250)', 'rgb(015,015,015)'];
	// blue
	var blue1 = ['rgb(031,112,149)', 'rgb(252,252,252)', 'rgb(236,236,236)', 'rgb(016,075,135)'];
	// blue w/ light border
	var blue2 = ['rgb(031,112,149)', 'rgb(250,250,250)', 'rgb(236,236,236)', 'rgb(199,199,199)'];
	
	var segoe = '"Segoe UI", "Lucida Grande", Tahoma, sans-serif';
	var arial = 'Arial, Helvetica, sans-serif';
	var lucinda = '"Lucida Sans Unicode", "Lucida Grande", sans-serif';

	//		      font    tab-border   row-border   h-padding   v-padding
	var table  = ['14px', '1px',       '1px',       '1px',		'1px'];
	var box    = ['14px', '1px',       '0px',       '1px',		'1px'];
	var spaced = ['14px', '1px',       '1px',       '2px',		'2px'];


	// *** USE PRESET SCHEMES HERE *** //
	var fontFamily = lucinda;
	var colors = blue2;
	var layout = table;
	// *** *** *** *** *** *** *** *** //


	// add css for suggestions div
	var suggestionsCss = document.createElement("style");
	suggestionsCss.type = "text/css";

	// add div CSS
	suggestionsCss.innerHTML = 
		'.autocorrect_suggestions { \n\
			position: absolute; \n\
			z-index: 1000; \n\
		} \n'

	// add table CSS
	suggestionsCss.innerHTML += 
		'.autocorrect_suggestions table { \n\
			border-collapse: collapse; \n\
    		border: solid ' + layout[1] + '; \n\
			\
			font-size: ' + layout[0] + '; \n\
        	font-family: ' + fontFamily + '; \n\
			\
			background-color:' +  colors[1] + '; \n\
			border-color:' +  colors[3] + '; \n\
		} \n';

	// add row CSS
	suggestionsCss.innerHTML += 
		'.autcorrect_suggestions th, td { \n\
    		border: solid ' + layout[2] + '; \n\
			padding-top: ' + layout[4] + '; \n\
			padding-bottom: ' + layout[4] + '; \n\
			padding-left: ' + layout[3] + '; \n\
			padding-right: ' + layout[3] + '; \n\
			\
			color:' +  colors[0] + '; \n\
			border-color:' +  colors[3] + '; \n\
		} \n';

	// add hover CSS
	suggestionsCss.innerHTML += 
		'.autocorrect_suggestions tr:hover { \n\
			background-color:' +  colors[2] + '; \n\
		} \n';

	// append css to document header
	document.head.appendChild(suggestionsCss);
}

// TODO ensure the initialisation doesn't happen twice. currently inputTAs are added twice
//
// called on module load
function moduleDidLoad() {
  	autocorrectModule = document.getElementById('autocorrect_module');

	if (testPage != undefined) {
		onTestPage = true;
	}

	addCss();

	// create suggestions element
    fragment = document.createDocumentFragment();

  	['input[type="text"]', 'textarea'].forEach(function (selector) {
		var inputBaseElement = document.querySelector(selector);

		inputElements.push(new TextInputElement(inputBaseElement));

		['keyup', 'click', 'scroll'].forEach(function (event) {
		 	inputElements[inputElements.length - 1].addEventListener(event, handleInputEvent);
		});
  	});
	
	console.log('moduleDidLoad(): ' + inputElements.length + 
			' text input elements found');


	// set handleKeyUp to be called ok keyup event
	//window.onkeyup = handleKeyUp;

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
	console.log('handleMessage() received: ' + receivedMessage);

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
	console.log('pageDidLoad(): page loaded');
}

// TODO: delete if not useful
//
// updates the status of the NaCL module
//function updateStatus(new_status) {
  	//if (new_status)
		//statusText = new_status;
//}

