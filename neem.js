//
//	I only made it OOP because I want this shit to be thread safe...
//			Neemscript
//
//

function Neem(stdout) {
	this.init = function() {
		this.instructions = [];
		this.vars = {};
		this.index = 0;
		this.eof = 0;
	}
	
	this.writetostdout = function(text) {
		stdout.value += text + '\n';
	}
	
	this.stdout = (stdout) ? this.writetostdout : console.log; //FOR TEXTBOXES AND SHIDD
	
	this.evalvars = function(str, classobj) {
		if(str === undefined) return "";
		var equals = (str.split("%").length -1);
		if(equals %2 != 0) {classobj.stdout("ERROR: Equals aren't paired: " + str); return;}
		var varbuffer = [];
		var addto = false;
		var newstr = str;
		for(var i in str) {
			if(str[i] === '%') {
				if(addto) {
					addto = false;
					var varname = varbuffer.join("");
					
					//SUBSTRING SHIT
					var sliceparams = varname.split(":~");
					var start = 0;
					var end = undefined;
					var varvalue = classobj.vars[sliceparams[0]];
					if(sliceparams.length == 2) {
						sliceparams[1] = sliceparams[1].split(',');
						start = (function(s, len) {
							if(s < 0) return len + s;
							return s;
						})(parseInt(sliceparams[1][0]), varvalue.length); //Get the proper start point
						if(sliceparams[1].length == 2) {
							end = parseInt(sliceparams[1][1]) + start;
						}
					}
					
					newstr = newstr.replace('%' + varname + '%', ((varvalue) ? varvalue.slice(start, end) : "" ));
				} else {
					addto = true;
					varbuffer = [];
				}
			}
			else if(addto) varbuffer.push(str[i]);
		}
		return newstr;
	}
	
	this.parseline = function(line, classobj) {
		var dict = {};
		
		if(line[0] == ':') { // DO THE LABELS HERE SINCE IT USES DIFFERENT CRITERIA
			if(line[1] == ':') return 1; //THIS IS A COMMENT -> ::comment
			dict.type = "label";
			dict.value = line.slice(1);
			dict.func = function(){};
			this.instructions.push(dict);
			return 1;
		}
		
		var splitted = line.split(" ");
		switch(splitted[0]) {
			case "echo":
				dict.type = "echo";
				dict.value = splitted.slice(1).join(" "); //everything after echo
				dict.func = function() {
					var str = classobj.evalvars(this.value, classobj);
					if(!str) return -1;
					classobj.stdout(str);
				};
				break;
			case "set":
				var temp = line.slice(4).split("="); //Get rid of "set " and then split the string
				dict.type = "set";
				dict.name = temp[0];
				dict.value = temp[1];
				dict.func = function() {classobj.vars[classobj.evalvars(this.name, classobj)] = classobj.evalvars(this.value, classobj)};
				break;
			case "goto":
				dict.type = "goto";
				dict.value = splitted.slice(1).join(" ").replace(/:/g, '');
				dict.func = function() {
					if(this.value == "eof") {var e = classobj.eof; classobj.eof = classobj.instructions.length; return e;}
					for(var i in classobj.instructions) {
						if(classobj.instructions[i].type == "label") {
							if(classobj.instructions[i].value == this.value) return i;
						}
					}
					console.log("ERROR: No label called " + this.value);
					return -1;
				}
				break;
			case "call":
				dict.type = "call";
				dict.value = splitted.slice(1).join(" ").replace(/:/g, '');
				dict.func = function() {
					classobj.eof = classobj.index;
					for(var i in classobj.instructions) {
						if(classobj.instructions[i].type == "label") {
							if(classobj.instructions[i].value == this.value) return i;
						}
					}
					console.log("ERROR: No label called " + this.value);
					return -1;
				}
				break;
			case "inc":
				dict.type = "inc";
				dict.name = splitted[1];
				dict.func = function() {
					var evalled = classobj.evalvars(this.name, classobj);
					var parsed = parseInt(classobj.vars[evalled]);
					if(isNaN(parsed)) {
						classobj.vars[evalled] = 0;
						return;
					}
					classobj.vars[evalled] = parsed + 1;
				}
				break;
			case "fi":
				dict.type = "fi"; //end of an IF; like bash
				dict.func = function(){}; //Just to make it consistent
				break;
			case "if":
				var possible = ["==", "!=", ">=", "<=", ">", "<", "!", "="];
				var temp = line.slice(3);
				for(var i in possible) {
					var z = temp.split(possible[i]);
					if(z.length == 2) {
						switch(possible[i]) {
							case "==":
								dict.check = function(classobj) {return classobj.evalvars(this.value.left, classobj) == classobj.evalvars(this.value.right, classobj)};
								break;
							case "=":
								dict.check = function(classobj) {return classobj.evalvars(this.value.left, classobj).toLowerCase() == classobj.evalvars(this.value.right, classobj).toLowerCase()};
								break;
							case "!=":
								dict.check = function(classobj) {return classobj.evalvars(this.value.left, classobj) != classobj.evalvars(this.value.right, classobj)};
								break;
							case "!":
								dict.check = function(classobj) {return classobj.evalvars(this.value.left, classobj).toLowerCase() != classobj.evalvars(this.value.right, classobj).toLowerCase()};
								break;
							case ">=":
								dict.check = function(classobj) {return parseFloat(classobj.evalvars(this.value.left, classobj)) >= parseFloat(classobj.evalvars(this.value.right, classobj))};
								break;
							case "<=":
								dict.check = function(classobj) {return parseFloat(classobj.evalvars(this.value.left, classobj)) <= parseFloat(classobj.evalvars(this.value.right, classobj))};
								break;
							case ">":
								dict.check = function(classobj) {return parseFloat(classobj.evalvars(this.value.left, classobj)) > parseFloat(classobj.evalvars(this.value.right, classobj))};
								break;
							case "<":
								dict.check = function(classobj) {return parseFloat(classobj.evalvars(this.value.left, classobj)) < parseFloat(classobj.evalvars(this.value.right, classobj))};
								break;
						}
						temp = z;
						break;
					}
				}
				if(!dict.check) {classobj.stdout("ERROR: Malformed if: " + line); return -1;}
				dict.type = "if";
				dict.value = {"left": temp[0], "right": temp[1]};
				dict.func = function() {
					if( this.check(classobj) ); //Just makes it less confusing to read, I feel, fuck
					else {
						for(var i = classobj.index, e = classobj.instructions.length; i < e; i++) {
							if(classobj.instructions[i].type == "fi") return i;
						}
						console.log("ERROR: No 'fi' for if statement");
						return -1;
					}
				};
				break;
			default:
				return 1;
		}
		if(dict.type) this.instructions.push(dict);
		return 1;
		//console.log(line);
	}
	
	this.interpret = function(text) {
		this.init();
		var lines = text.split("\n");
		for(var i in lines) if(this.parseline(lines[i], this) == -1) return;
		
		this.eof = this.instructions.length; //Setup just in case
		for(e = this.eof; this.index < e; this.index++) {
			var ret = this.instructions[this.index].func();
			if(ret >= 0) this.index = ret; //For loops and shit
			else if(ret == -1) return; //Something's fucked
		}
		//this.stdout("Done");
	}
}