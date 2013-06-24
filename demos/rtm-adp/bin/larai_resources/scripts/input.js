//Read an input on the console
read = function(message){
	if(message != undefined)
		print(message.toString());
	var inp = new java.util.Scanner('java.lang.System.in');
	return inp.nextLine();
}

//Load a script
 load = function(path){ 

	 eval(String(readFile(path))) 

 } 

 //Read a file
 function readFile(path){ 

                 stream = new java.io.FileInputStream(new java.io.File(path)); 

                 fc = stream.getChannel(); 

                 bb = fc.map(java.nio.channels.FileChannel.MapMode.READ_ONLY, 0, fc.size()); 

                 return java.nio.charset.Charset.defaultCharset().decode(bb).toString(); 

             }