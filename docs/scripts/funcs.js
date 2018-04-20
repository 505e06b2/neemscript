function resizedwindow() {
	document.getElementById("stylesheet").href = "styles/" + ((window.innerWidth < window.innerHeight) ? "mobile.css" : "desktop.css");
	mobile = (window.innerWidth < window.innerHeight) ? true : false;
}

function beenedited(val = true) {
	if(val) {
		edited = true;
		document.title = "* Neemscript";
	} else {
		edited = false;
		document.title = "Neemscript";
	}
}

function runcode(script) {
	if(interpret === undefined) {
		alert('Can\'t load Neemscript... Try refreshing');
		return;
	}
	if(mobile) outbox.focus();
	Module.print("[*] Running script... This could take a while...");
	savecurrentfile(function() {
		Module.print("     ======= START =======");
		interpret(editfilename.innerHTML);
		Module.print("     ======== END ========");
		savefiles();
	});
}

function resetqr() {
	document.getElementById("replacescript").disabled = true;
	document.getElementById("runscript").disabled = true;
	qrbox.value = "";
}

function openfileselect() {
	fileselect.click();
	return false;
}

function changeCamera(id) {
	Instascan.Camera.getCameras().then(function (cameras) {
		if (cameras.length > 0) {
			currentcam = cameras[id];
			localStorage.camamount = cameras.length;
			localStorage.currentcam = id;
			scanner.start(currentcam);
		}
	});
}

function toggleqr() {
	var qrwindow = document.getElementById("qrwindow");
	if(qrwindow.style.display == "none") {
		qrwindow.style.display = "block";
		scanner.start(currentcam);
	} else {
		qrwindow.style.display = "none";
		scanner.stop();
	}
}
