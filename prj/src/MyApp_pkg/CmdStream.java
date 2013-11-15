package MyApp_pkg;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.DRAMCommandStream;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.CounterChain;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;

public class CmdStream extends Kernel {
    public CmdStream(KernelParameters parameters) {
      super(parameters);

      HWVar totalBursts   =io.scalarInput("totalBursts",hwUInt(32));
      HWVar wordsPerBurst =io.scalarInput("wordsPerBurst",hwUInt(32));
      
      CounterChain chain = control.count.makeCounterChain();
      HWVar burstCount = chain.addCounter(totalBursts,1);
      HWVar wordCount = chain.addCounter(wordsPerBurst,1);

      DRAMCommandStream.makeKernelOutput("dram_read",
          wordCount.eq(0),        // control
          burstCount,                      // address
          constant.var(hwUInt(8), 1),      // size
          constant.var(hwUInt(1), 0),      // inc
          constant.var(hwUInt(1), 0),      // stream
          constant.var(false));

//    DRAMCommandStream.makeKernelOutput("dram_write",
//        wordCount.eq(0),        control
//        burstCount,                      address
//        constant.var(hwUInt(8), 1),      size
//        constant.var(hwUInt(1), 0),      inc
//        constant.var(hwUInt(1), 0),      stream
//        constant.var(false));
          //burstCount.eq(totalBursts-1));
                                         

    debug.printf("cycle:%d-%d\n",burstCount,wordCount);

    }
}
