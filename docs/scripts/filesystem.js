function fserror(err) {
	alert(err);
	console.error(err);
	return false;
}

function savefiles(filename, func) {
	FS.syncfs(function(e) {
		if(e) {console.error(e); return false;}
		//Module.print("[*] Synced filesystem to browser");
		updatefilelist(filename);
		if(func) func();
	});
	return false;
}

function renamefile(filename) {
	var newpath = window.prompt("Enter new filename:");
	if(!newpath) return false;
	try {
		FS.rename(filename, newpath)
	} catch (err) {
		return fserror(err);
	}
	if(filename == editfilename.innerHTML) {
		editfile(newpath);
	}
	savefiles();
	return false;
}

function editfile(filename) {
	if(edited && confirm("Save file?")) savecurrentfile();
	try {
		editor.session.setValue(FS.readFile(filename, {encoding: "utf8"}));
	} catch (err) {
		return fserror(err);
	}
	editfilename.innerHTML = filename;
	localStorage.lastedited = filename;
	Module.print("[*] Loaded '" + filename + "' for editing");
	setTimeout(function() {beenedited(false)}, 50);
	return false;
}

function exportfile() {
	var blob = "";
	outputfile.download = editfilename.innerHTML;
	try {
		blob = new Blob([
			FS.readFile(outputfile.download, {encoding: "utf8"})
		], { type: 'text/plain' });
	} catch (err) {
		return fserror(err);
	}
	outputfile.href = (window.webkitURL || window.URL).createObjectURL(blob);
	outputfile.dataset.downloadurl = ['text/plain', outputfile.download, outputfile.href].join(':');
	outputfile.click();
	return false;
}

function importfileselect(obj) {
	if(obj.files.length === 0) return;
	var reader = new FileReader();
	reader.filename = obj.files[0].name;
	reader.onload = function() {
		FS.writeFile(reader.filename, reader.result);
		savefiles(reader.filename, function() {
			editfile(reader.filename);
		});
	};
	reader.readAsText(obj.files[0], "utf8");
	obj.value = "";
}

function savecurrentfile(func) {
	try {
		FS.writeFile(editfilename.innerHTML, editor.getValue());
	} catch (err) {
		return fserror(err);
	}
	beenedited(false);
	Module.print("[*] Saved '" + editfilename.innerHTML + "'");
	savefiles(null, func);
	return false;
}

function deletefile(filename) {
	if(!confirm("Are you sure you want to delete '" + filename + "'?")) return false;
	try {
		FS.unlink(filename);
	} catch (err) {
		return fserror(err);
	}
	
	if(editfilename.innerHTML == filename) {
		var newfilename = FS.readdir(".");
		if(newfilename.length > 2) editfile(newfilename[2]);
	}
	Module.print("[*] Deleted '" + filename + "'");
	
	savefiles();
	return false;
}

function updatefilelist(focusto) {
	var dirlist = FS.readdir(filesystemroot);
	if(dirlist.length <= 2) {
		createdefaultfile();
		return false;
	}
	dirlist.sort();
	
	fstable.innerHTML = "";
	for(var i = 2; i < dirlist.length; i++) { //remove . and ..
		fstable.innerHTML += ((focusto == dirlist[i]) ? "<div id='filelist_focus' tabindex='-1'>" : "<div>") + 
			"<a href='#Edit File' title='Edit' onclick='return editfile(this.innerHTML)'>" + dirlist[i] + "</a>" +
			"<div class='righticon'>" +
				"<a href='#Export File' title='Export' style='color: var(--blue)' onclick='return exportfile(\"" + dirlist[i] + "\")'>[Ex]</a>" + 
				"<a href='#Rename File' title='Rename' style='color: var(--green)' onclick='return renamefile(\"" + dirlist[i] + "\")'>[Ren]</a>" + 
				"<a href='#Delete File' title='Delete' style='color: var(--red)' onclick='return deletefile(\"" + dirlist[i] + "\")'>[Del]</a>" + 
			"</div>" +
		"</div>";
	}
	if(focusto) {
		var elem = document.getElementById("filelist_focus");
		elem.focus();
		elem.removeAttribute("tabindex");
		elem.removeAttribute("id");
	}
	return false;
}

function addfile() {
	var filename = window.prompt("Enter Filename:");
	if(!filename) return false;
	try {
		FS.create(filename);
	} catch (err){
		return fserror(err);
	}
	editfile(filename);
	Module.print("[*] Create '" + filename + "'");
	savefiles(filename);
	return false;
}

function createdefaultfile() {
	const filename = "main.neem";
	try {
		var rawFile = new XMLHttpRequest();
		rawFile.onreadystatechange = function () {
			if(rawFile.readyState === 4 && rawFile.status === 200) FS.writeFile(filename, rawFile.responseText);
		}
		rawFile.open("GET", "examples/" + filename);
		rawFile.send();
	} catch (err) {
		return fserror(err);
	}
	Module.print("[*] No files on the system; generating a sample script");
	savefiles(null, function() {
		editfile(filename);
	});
}
