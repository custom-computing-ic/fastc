package engine;

import com.maxeler.maxcompiler.v1.managers.MAXBoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v1.managers.custom.Stream;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.KernelBlock;
import static com.maxeler.maxcompiler.v1.managers.custom.stdlib.MemoryControlGroup.MemoryAccessPattern.LINEAR_1D;
import com.maxeler.maxcompiler.v1.managers.custom.ManagerClock;

import com.maxeler.maxcompiler.v1.managers.custom.stdlib.MaxRingBidirectionalStream;
import com.maxeler.maxcompiler.v1.managers.custom.stdlib.Max3RingConnection;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.Mux;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.Demux;

import com.maxeler.maxcompiler.v1.managers.custom.blocks.StateMachineBlock;
import com.maxeler.maxcompiler.v1.statemachine.manager.ManagerStateMachine;
import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;

public class AppManager extends CustomManager {

    AppManager(MAXBoardModel board_model, String name, Target target) {
        super(board_model, name, target);

        //  connection between CPU and FPGA
        // create streams between host and manager
        Stream host2mgr = addStreamFromHost("host2mgr");
        Stream mgr2host = addStreamToHost("mgr2host");

        KernelBlock hostr   = addKernel(new Cmdread(makeKernelParameters("Cmdhostread")));
        KernelBlock hostw   = addKernel(new Cmdwrite(makeKernelParameters("Cmdhostwrite")));

        // create streams between DRAM and manager
        Stream dram2mgr = addStreamFromOnCardMemory("dram2mgr", hostr.getOutput("dram_read"));
        Stream mgr2dram = addStreamToOnCardMemory("mgr2dram",   hostw.getOutput("dram_write"));

        // connect the streams between host and external memory
        mgr2dram.connect(host2mgr);
        mgr2host.connect(dram2mgr);

        //  FPGA-0
        KernelBlock k0   = addKernel(new Convolution1d(makeKernelParameters("MyApp")));
        KernelBlock r0   = addKernel(new Cmdread(makeKernelParameters("Cmdread0")));
        KernelBlock w0   = addKernel(new Cmdwrite(makeKernelParameters("Cmdwrite0")));

        Stream dram2knl0 = addStreamFromOnCardMemory("dram2knl0",  r0.getOutput("dram_read"));
        Stream knl2dram0 = addStreamToOnCardMemory("knl2dram0",    w0.getOutput("dram_write"));


        // create streams between DRAM and kernel
        k0.getInput("p").connect(dram2knl0);
        knl2dram0.connect(k0.getOutput("output"));
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
