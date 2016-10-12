// ./Google\ Chrome --user-data-dir=/Users/mj/chrome_logging


function TestPage() {
	var keyUpTime;
	var messageReceivedTime;

	//constructor() {
		//keyUpTime = 0;
		//messageReceivedTime = 0;
	//}

	// displays the suggestions under the textarea on the test page
	this.printSuggestions = function(suggestionsStr) {
		document.getElementById('suggestions').innerHTML = 'words: ' + suggestionsStr;
	}

	this.printTimeTaken = function() {
		document.getElementById('time_taken').innerHTML = 'time: ' + 
			(messageReceivedTime - keyUpTime) + 'ms';
	}

	this.clearTimeTaken = function() {
		document.getElementById('time_taken').innerHTML = 'time: -';
	}

	this.setKeyUpTime = function(newKeyUpTime) {
		keyUpTime = newKeyUpTime;
	}

	this.setMessageReceivedTime = function(newMessageReceivedTime) {
		messageReceivedTime = newMessageReceivedTime;
	}
};

var testPage = new TestPage();
