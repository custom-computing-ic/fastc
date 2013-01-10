package engine_orig;

import com.maxeler.maxcompiler.v1.managers.MAXBoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v1.managers.custom.Stream;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.KernelBlock;
import static com.maxeler.maxcompiler.v1.managers.custom.stdlib.MemoryControlGroup.MemoryAccessPattern.LINEAR_1D;
import com.maxeler.maxcompiler.v1.managers.custom.ManagerClock;

//MaxRing
import com.maxeler.maxcompiler.v1.managers.custom.stdlib.MaxRingBidirectionalStream;
import com.maxeler.maxcompiler.v1.managers.custom.stdlib.Max3RingConnection;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.Mux;
import com.maxeler.maxcompiler.v1.managers.custom.blocks.Demux;

//SM
import com.maxeler.maxcompiler.v1.managers.custom.blocks.StateMachineBlock;
import com.maxeler.maxcompiler.v1.statemachine.manager.ManagerStateMachine;


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
    KernelBlock k0   = addKernel(new MyApp(makeKernelParameters("MyApp0")));

    KernelBlock r0   = addKernel(new Cmdread(makeKernelParameters("Cmdread0")));
    KernelBlock r1   = addKernel(new Cmdread(makeKernelParameters("Cmdread1")));
    KernelBlock r2   = addKernel(new Cmdread(makeKernelParameters("Cmdread2")));
    KernelBlock r3   = addKernel(new Cmdread(makeKernelParameters("Cmdread3")));

    KernelBlock w0   = addKernel(new Cmdwrite(makeKernelParameters("Cmdwrite0")));
    KernelBlock w1   = addKernel(new Cmdwrite(makeKernelParameters("Cmdwrite1")));

    Stream dram2knl0 = addStreamFromOnCardMemory("dram2knl0",  r0.getOutput("dram_read"));
    Stream dram2knl1 = addStreamFromOnCardMemory("dram2knl1",  r1.getOutput("dram_read"));
    Stream dram2knl2 = addStreamFromOnCardMemory("dram2knl2",  r2.getOutput("dram_read"));
    Stream dram2knl3 = addStreamFromOnCardMemory("dram2knl3",  r3.getOutput("dram_read"));

    Stream knl2dram0 = addStreamToOnCardMemory("knl2dram0",    w0.getOutput("dram_write"));
    Stream knl2dram1 = addStreamToOnCardMemory("knl2dram1",    w1.getOutput("dram_write"));

// create streams between DRAM and kernel
    k0.getInput("burst_p").connect(dram2knl0);
    k0.getInput("burst_pp").connect(dram2knl1);
    k0.getInput("burst_dvv").connect(dram2knl2);
    k0.getInput("burst_source").connect(dram2knl3);

    knl2dram0.connect(k0.getOutput("ker_p"));
    knl2dram1.connect(k0.getOutput("output_pp"));
	}
}
