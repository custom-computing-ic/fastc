//Print a message
function print (message){
	java.lang.System.out.print(message)
	}

//Print a message and ends it with a new line
function println(message){
	java.lang.System.out.println(message)
	}

//Print an error message
function error (message){
	java.lang.System.err.print(message)
}
//Print an error message and ends it with a new line	
function errorln (message){
	java.lang.System.err.println(message)
	}
	
function printObject (obj, space){
	if(obj == undefined)
		return;
	if(space == undefined)
		space = "";
	for(var prop in obj){
		if(obj.constructor==Array){
			print(space+"["+prop+"] ");
		}else print(space+prop+": ");
		if(  obj[prop] == undefined){
			println("undefined");
			return;
		}
		if ( obj[prop].constructor==Object || obj[prop].constructor==Array){
			println("");
			printObject(obj[prop], space+"  ");
		} else	if ( obj[prop].constructor==Function){
			println(obj[prop].toSource());
		} else{
			print(obj[prop]);
			println("");
		}
	}
}

//Insert save to file functions (and others) here!
importClass(Packages.larac.utils.ReadWriteTextFile)
function saveFile(fileName, content){
	ReadWriteTextFile.setContents(new java.io.File(fileName),content);
}
