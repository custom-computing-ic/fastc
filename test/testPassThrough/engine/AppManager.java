package engine;

import com.maxeler.maxcompiler.v1.managers.MAXBoardModel;
import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.KernelBlock;

public class AppManager extends CustomManager {
  AppManager(MAXBoardModel board_model, String name, Target target) {
    super(board_model, name, target);

   KernelBlock k = addKernel(new PassThroughKernel(makeKernelParameters("PassThroughKernel")));

   // connect all inputs
   for (String input : k.getAllInputs())
     k.getInput(input) <== addStreamFromHost(input);
  
   // connect all outputs
   for (String output : k.getAllOutputs())
     addStreamToHost(output) <== k.getOutput(output);
 
   // configure the manager
   config.setDefaultStreamClockFrequency(300);   // MHZ
  }

  public static void main(String [] args) {
    if (args.length < 2 ) {
     System.out.println("Usage: AppManager <BuildName> SIM|HW");
     System.exit(1);
    }

    String name = args[0];
    CustomManager.Target target = "SIM".equals(args[1]) ? 
         Target.MAXFILE_FOR_SIMULATION : Target.MAXFILE_FOR_HARDWARE;

    AppManager m = new AppManager(MAX3BoardModel.MAX3424A, name, target);
    m.build();
  }

}    
