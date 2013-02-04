package engine_orig;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.DRAMCommandStream;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.Counter;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.WrapMode;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.Params;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;

public class Cmdwrite extends Kernel {
    public Cmdwrite(KernelParameters parameters) {
      super(parameters);
      int   Burst_inc     =1;

      HWVar iniBursts     =io.scalarInput("iniBursts",hwUInt(32));
      HWVar iterations    =io.scalarInput("iterations",hwUInt(32));
      HWVar totalBursts   =io.scalarInput("totalBursts",hwUInt(32));
      HWVar wordsPerBurst =io.scalarInput("wordsPerBurst",hwUInt(32));
      HWVar Enable        =io.scalarInput("Enable",hwUInt(1));

      //address generator
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

      Count.Params param2 = control.count.makeParams(32)
           .withEnable(counter1.getWrap())
           .withMax(iterations)
           .withInc(1);
      Counter counter2    = control.count.makeCounter(param2);
      HWVar iterCount     = counter2.getCount();

      HWVar Control       = wordCount.eq(0) & Enable;

      DRAMCommandStream.makeKernelOutput("dram_write",
          Control,                          // control
          burstCount+iniBursts,             // address
          constant.var(hwUInt(8),Burst_inc),// size
          constant.var(hwUInt(6),  1),      // inc
          constant.var(hwUInt(4),  0),      // stream
          burstCount.eq(totalBursts - Burst_inc) & iterCount.eq(iterations - 1));


//------------------------------The debug part--------------------------------
      debug.printf("Address%ld:%d-%d  %d\n",iniBursts, burstCount, wordCount, totalBursts + iniBursts - Burst_inc);
//    debug.printf("Unsyn_cycles:%d\n", unsynCount);

    }
}
