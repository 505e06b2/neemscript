function init_qrscanner() {
	scanner = new Instascan.Scanner({ video: document.getElementById("qrpreview"), mirror: !mobile });
	scanner.addListener('scan', function (content) {
        qrbox.value = content;
		document.getElementById("replacescript").disabled = false;
		document.getElementById("runscript").disabled = false;
    });
	Instascan.Camera.getCameras().then(function (cameras) {
		if (cameras.length > 0) {
			var camselect = document.getElementById("cameraselect");
			var index = (localStorage.currentcam && localStorage.camamount == cameras.length) ? localStorage.currentcam : 0;
			camselect.innerHTML = "";
			for(var i = 0; i < cameras.length; i++) {
				camselect.innerHTML += "<option value='" + i + "'" + ((i == index) ? " selected " : "") + ">" + cameras[i].name + "</option>";
			}
			currentcam = cameras[index];
		} else {
			console.error('No cameras found.');
		}
	}).catch(function (e) {
		console.error(e);
	});
}

function init_neemscript() {
	var neemloader = document.createElement("script");
	neemloader.onload = function() {
		Module.print = function(text) {
			outbox.value += text + '\n';
			outbox.scrollTop = outbox.scrollHeight;
		};	
		Module.printErr = Module.print;
	
		Module.onRuntimeInitialized = function() {
			interpret = Module.cwrap('webmain', null, ['string']);
			console.log("Loaded Neemscript");
			Module.print("[*] Neemscript runtime loaded");
			init_filesystem();
		}
	}
	
	var t = (typeof WebAssembly === "undefined") ? "asm.js" : "wasm";
	neemloader.src = "neemscript" + ((typeof WebAssembly === "undefined") ? ".js" : "_wasm.js");
	outbox.value += "\n[*] Using " + t + " for Neemscript\n";
	document.head.appendChild(neemloader);
}

function init_filesystem() {
	//FS.mkdir(filesystemroot);
	FS.mount(IDBFS, {}, filesystemroot);
	FS.syncfs(true, function (err) {
		if(err) {console.error(err); return;};
		FS.chdir(filesystemroot);
		updatefilelist();
		if(localStorage.lastedited) editfile(localStorage.lastedited);
	});
}
