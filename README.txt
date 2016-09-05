Build instructions:
  1 Navigate to autocorrect_module_src
  2 Modify Makefile so NACL_SDK_ROOT points to a pepper toolchain directory 
  	(available at: https://developer.chrome.com/native-client/sdk/download)
  3 make
  4 cp ./pnacl/Release/autocorrect_module.pexe ../extension/autocorrect_module/
  5 In google chrome: Window > Extensions > Developer mode (tick)
  6 Load "extension" folder as an unpacked extension
  7 Click the extension's "T" icon to open the test page
