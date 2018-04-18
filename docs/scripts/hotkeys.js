function hotkey_check(e) {
	if(e.keyCode == 9) { //Tab
		stopaction(e);
		var s = this.selectionStart;
		this.value = this.value.substring(0,this.selectionStart) + "\t" + this.value.substring(this.selectionEnd);
		this.selectionEnd = s+1; 
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