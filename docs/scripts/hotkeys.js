function hotkey_check(e) {
	if(e.keyCode == 9) { //Tab
		stopaction(e);
		this.value += "\t";
	} else if(e.ctrlKey) {
		if(e.keyCode == 81) { //Ctrl + Q
			stopaction(e);
			runcode(inbox.value);
		} else if(e.keyCode == 83) { //Ctrl + S
			stopaction(e);
			savecurrentfile()
		} else if(e.keyCode == 79) { //Ctrl + O
			stopaction(e);
			editfile(editfilename.innerHTML);
		} else if(e.keyCode == 87) { //Ctrl + W
			stopaction(e);
			outbox.value = "[*] Cleared Output\n";
		}
	}
}