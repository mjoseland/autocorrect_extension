chrome.browserAction.onClicked.addListener(function(tab) {
  //alert("alert");
  var newURL = "autocorrect_test_page.html";
    chrome.tabs.create({ url: newURL });
});
