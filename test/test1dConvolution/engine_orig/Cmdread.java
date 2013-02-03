package engine_orig;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.DRAMCommandStream;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.WrapMode;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.Counter;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.Params;


public class Cmdread extends Kernel {
    public Cmdread(KernelParameters parameters) {
      super(parameters);
      int   Burst_inc     =1;
            
      HWVar iniBursts     =io.scalarInput("iniBursts",hwUInt(32));
      HWVar totalBursts   =io.scalarInput("totalBursts",hwUInt(32));
      HWVar wordsPerBurst =io.scalarInput("wordsPerBurst",hwUInt(32));
      HWVar Enable        =io.scalarInput("Enable",hwUInt(1));
      
      //the address counters
      Count.Params param0 = control.count.makeParams(32)
           .withEnable(Enable)
           .withMax(wordsPerBurst)
           .withInc(1);
      Counter counter0    = control.count.makeCounter(param0);
      HWVar  wordCount    = counter0.getCount();

      Count.Params param1 = control.count.makeParams(32)
           .withEnable(counter0.getWrap())
           .withMax(totalBursts)
           .withInc(Burst_inc);
      Counter counter1    = control.count.makeCounter(param1);
      HWVar burstCount    = counter1.getCount();   
      
      HWVar Control       = wordCount.eq(0) & Enable;

      DRAMCommandStream.makeKernelOutput("dram_read",
          Control,                          // control
          burstCount+iniBursts,             // address
          constant.var(hwUInt(8),Burst_inc),// size
          constant.var(hwUInt(6),  1),      // inc
          constant.var(hwUInt(4),  0),      // stream
          constant.var(false));
      

//------------------------------The debug part--------------------------------
//    debug.printf("Address%d:%d-%d\n", iniBursts, burstCount, wordCount);
//    debug.printf("Unsyn_cycles:%d\n", unsynCount);
//    debug.printf("Control:%d\n",Control);

    }
}
