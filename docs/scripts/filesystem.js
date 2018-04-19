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
	try {
		editor.session.setValue(FS.readFile(filename, {encoding: "utf8"}));
	} catch (err) {
		return fserror(err);
	}
	editfilename.innerHTML = filename;
	localStorage.lastedited = filename;
	Module.print("[*] Loaded '" + filename + "' for editing");
	return false;
}

function savecurrentfile() {
	try {
		FS.writeFile(editfilename.innerHTML, editor.getValue());
	} catch (err) {
		return fserror(err);
	}
	Module.print("[*] Saved '" + editfilename.innerHTML + "'");
	savefiles();
	return false;
}

function deletefile(filename) {
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
			dirlist[i] + 
			"<div class='righticon'>" +
				"<a href='#Edit File' title='Edit' style='color: var(--blue)' onclick='return editfile(\"" + dirlist[i] + "\")'>[Edit]</a>" + 
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
		FS.writeFile(filename,
"::Quick script anyone could make but shows quite a few features\n" +
"::Check the wiki to see how you can make your own scripts\n" +
"\n" +
"output tempdata w\n" +
"set i=0\n"+
":label\n" +
"	if %i%==5\n" +
"    goto :end\n" +
"  fi\n" +
"  inc i\n" +
"  echo %i%\n" +
"  goto :label\n" +
"\n" +
":end\n" +
"  output reset\n" +
"  echo Finished; going to read and print what was written:\n" +
"  input tempdata\n" +
"  readall temp\n" +
"  echo %temp%\n"
		);
	} catch (err) {
		return fserror(err);
	}
	Module.print("[*] No files on the system; generating a sample script");
	savefiles(null, function() {
		editfile(filename);
	});
}
