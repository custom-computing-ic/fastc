if(tools == undefined)
	var tools = new Object();

tools.xilinx = {

	init: function(args){
		with(this){
			if(args == undefined){
				if(	 tools.xilinx.design == undefined
				   ||
				     tools.xilinx.pathFolder == undefined
				   //||
				   //	 org.reflect.xilinx.XilinxDefs.pathISE == undefined
				   ||
				     tools.xilinx.rt == undefined

				   )
					 throw("Undefined parameters for xilinx tool");
			     else return;
			} 

			if(args["design"] == undefined){
				if(tools.xilinx.design == undefined)
					throw("No design name defined for running the tool!");
				else println("Using design: "+tools.xilinx.design+" as default.");
			} else tools.xilinx.design = args["design"];
			
			if(args["pathFolder"] == undefined){
				if(tools.xilinx.pathFolder == undefined)
					throw("No pathFolder defined for running the tool!");
				else println("Using pathFolder: "+tools.xilinx.pathFolder+" as default.");
			} else tools.xilinx.pathFolder = args["pathFolder"];
			
			if(args["pathISE"] != undefined)
				org.reflect.xilinx.XilinxDefs.pathISE = args["pathISE"];

		      	if(tools.xilinx.rt == undefined) {
                                tools.xilinx.rt = new org.reflect.xilinx.RunTools(pathFolder);
                        }               
                                

 			if(args["device"] != undefined)
                        	tools.xilinx.rt.myXSTDefs.setFPGA(args["device"]);

      			//if(args["device"] != undefined)
                         //    tools.xilinx.rt.setFPGA(args["device"]);

			if(args["cores"] != undefined)
                                tools.xilinx.rt.setCoresDir(args["cores"]);
		  }
	},

	xst: function(args){
		with(tools.xilinx){
			init(args);
			//if( tools.xilinx.XstOptions == null)
			//	tools.xilinx.XstOptions = new java.util.HashMap();
			
			if(args["xstOptions"] != undefined) {
				if(tools.xilinx.xstOptions == null) {
                                	tools.xilinx.xstOptions = new java.util.HashMap();
				} else {
					tools.xilinx.xstOptions.clear();
				}

				for(var __opt__ in args.xstOptions) {
					tools.xilinx.xstOptions.put(__opt__, args.xstOptions[__opt__]);
					//println("xst option: "+__opt__+" value = "+args.xstOptions[__opt__]);
				}
			
				//println("defining XST options");	
				rt.setXstOptions(tools.xilinx.xstOptions);
			}

			if(args["device"] == undefined) {
				//println("calling synth without device name!");
				rt.synth(args.design);
			} else {
				rt.synth(args.design, args.device);
			}

			if((args.clkName != undefined) && (args.ctrMaxFreq != undefined))
				rt.constraints(args.design, args.clkName, args.ctrMaxFreq);

			//println("NOW COMES  the REPORT");
			var __js__ = ""+(new org.reflect.xilinx.Report()).getXSTReportStr(rt.workingFolderStr,"design", design);
			//println("REPORT: \n"+__js__);
			//return (new org.reflect.xilinx.Report()).getXSTReportStr(rt.workingFolderStr,"design", design);
			//__js__ =  'attributes.set({design:{CCU:{maxFreq:"2"}}})';
			 //println("REPORT: \n"+__js__);

			return __js__;

			/*
			println('############################################');
			println(__js__);
			println('############################################');
			
			var newJS = 'attributes.set({'+
								'function:{'+
										'MajorityVoter:{'+
											'msgWarnings:"0",'+
											'device:"5vlx50tff1136",'+
											'msgInfos:"0",'+
											'delay:"20.167",'+
											'numSliceLUTs:"597",'+
											'msgErrors:"0",'+
										'}'+
								'}'+
						'});';
						println(newJS);
			return newJS;
/**/	
		}
	},

	constraint: function(args) {
		 with(tools.xilinx){
                        init(args);

			var _js_;
                        if((args["clkName"] != undefined) && (args["ctrMaxFreq"] != undefined)) {
				 rt.constraints(args.design, args.clkName, args.ctrMaxFreq);
				 _js_ = 'attributes.set({design:{'+
                                                            args.design+':{'+
							'ctrMaxFreq:'+ctrMaxFreq+','+
							'clkName:"'+clkName+'",'+
							'}'
							'}'
							'});'
				printObject(attributes);

                        } else {
				println("Undefined constraints!");
			}
			return _js_;	
		}
	},

	xpwr: function(args){
		with(tools.xilinx){
			init(args);

 			if(args.xpwrOptions != undefined) {
                                if( tools.xilinx.xpwrOptions == null)
                                        tools.xilinx.xpwrOptions = new java.util.HashMap();
				else tools.xilinx.xpwrOptions.clear();

                                for(var __opt__ in args.xpwrOptions)
                                        xpwrOptions.put(__opt__, args.xpwrOptions[__opt__]);

                                rt.setXpwrOptions(xpwrOptions);
                        }

			rt.xpwr(design);
			var __js__ = ""+(new org.reflect.xilinx.Report()).getXPRReportStr(
	                      rt.workingFolderStr, "design", design);
			return __js__;
//			return 'attributes.set({ function:{ MajorityVoter:{ device:"5vlx50tff1136"}}});';
		}
	},


	ngdbuild: function(args){

		with(tools.xilinx){
			init(args);
 
			if(args.ngdOptions != undefined) {
                                if( tools.xilinx.ngdOptions == null)
                                        tools.xilinx.ngdOptions = new java.util.HashMap();
				else tools.xilinx.ngdOptions.clear();

                                for(var __opt__ in args.ngdOptions)
                                        ngdOptions.put(__opt__, args.ngdOptions[__opt__]);

				rt.setNgdOptions(ngdOptions);
                        }

			rt.buildNgd(design, rt.ucfFile); // ucFile can be null
//			return 'attributes.set({ function:{ MajorityVoter:{ msgInfos:"0"}}});';
		}
	},
 
	map: function(args){
		with(tools.xilinx){
			init(args);

			if(args.mapOptions != undefined) {
                                if( tools.xilinx.mapOptions == null)
                                        tools.xilinx.mapOptions = new java.util.HashMap();
				else tools.xilinx.mapOptions.clear();

                                for(var __opt__ in args.mapOptions)
                                        mapOptions.put(__opt__, args.mapOptions[__opt__]);

				rt.setMapOptions(mapOptions);
                        }       

			 rt.map(design);
			 
			var __js__ = ""+(new org.reflect.xilinx.Report()).getMAPReportStr(
				 	rt.workingFolderStr, "design", design);
			return __js__;
//			return 'attributes.set({ function:{ MajorityVoter:{ delay:"20.167"}}});';
		}
	},

	par: function(args){
		with(tools.xilinx){
			init(args);

			if(args.parOptions != undefined) {
                                if( tools.xilinx.parOptions == null)
                                        tools.xilinx.parOptions = new java.util.HashMap();
				else tools.xilinx.parOptions.clear();

                                for(var __opt__ in args.parOptions)
                                        mapOptions.put(__opt__, args.parOptions[__opt__]);

                                rt.setParOptions(parOptions);
                        }

			rt.par(design);
			var __js__ = ""+(new org.reflect.xilinx.Report()).getPARReportStr(
					rt.workingFolderStr, "design", design);
			return __js__;
		}
	}

	// bitgen and tcre (to be included later)
}
