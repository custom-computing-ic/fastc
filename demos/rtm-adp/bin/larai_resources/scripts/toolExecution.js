function run(tool, args, jpname, name, verbose){
	args = (args== undefined)?[]:args;
	jpname = (jpname== undefined)?null:jpname;
	name = (name== undefined)?null:name;
	verbose = (verbose == undefined)?0:verbose;
	var argsToSource = "";
	try{
		argsToSource = args.toSource();
	}catch(e){
	}
	var report = "";
	try{
		report = ""+System.run(tool,args,jpname, name, verbose, argsToSource);
	}catch(e){
			errorln(e);
			throw null;
	}
	eval(report);
}

function cmd(command, args, verbose){
	args = (args== undefined)?[]:args;
	verbose = (verbose == undefined)?0:verbose;
	System.execute(command,args,verbose);
}

function report(tool, args, verbose){
	args = (args== undefined)?[]:args;
	verbose = (verbose == undefined)?0:verbose;
	var argsToSource = "";
	try{
		argsToSource = args.toSource();
	}catch(e){
	}
	var reportStr = "";
	try{
		 reportStr = ''+System.report(tool,args, argsToSource,verbose);
	}catch(e){
			errorln(e);
			throw null;
	}
	eval(reportStr);
}

function toJavaArray(classType, array){
	var argv = java.lang.reflect.Array.newInstance(classType, array.length);
	for(var i = 0; i < array.length; i++)
		java.lang.reflect.Array.set(argv, i, array[i]);
	return argv;
}

function export(varName,value){
	if(value == undefined)
		System.export(varName);
	else System.export(varName,value);
	};

