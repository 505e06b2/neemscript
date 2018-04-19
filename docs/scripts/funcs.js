function resizedwindow() {
	document.getElementById("stylesheet").href = "styles/" + ((window.innerWidth < window.innerHeight) ? "mobile.css" : "desktop.css");
	mobile = (window.innerWidth < window.innerHeight) ? true : false;
}

function runcode(script) {
	if(interpret === undefined) {
		alert('Can\'t load Neemscript... Try refreshing');
		return;
	}
	outbox.focus();
	Module.print("[*] Running script... This could take a while...");
	Module.print("     ======= START =======");
	setTimeout(function() {
		interpret(script);
		Module.print("     ======== END ========");
		savefiles();
	}, 100);
}

function resetqr() {
	document.getElementById("replacescript").disabled = true;
	document.getElementById("runscript").disabled = true;
	qrbox.value = "";
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
