package engine;

import com.maxeler.maxcompiler.v1.kernelcompiler.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.*    ;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.KernelMath.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Stream.OffsetExpr;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.*;

public class Cmdwrite extends Kernel {

public Cmdwrite(KernelParameters parameters) {
super(parameters);
HWVar iniBursts =  io.scalarInput("iniBursts", hwUInt(32));
HWVar iterations =  io.scalarInput("iterations", hwUInt(32));
HWVar totalBursts =  io.scalarInput("totalBursts", hwUInt(32));
HWVar wordsPerBurst =  io.scalarInput("wordsPerBurst", hwUInt(32));
HWVar Enable =  io.scalarInput("Enable", hwUInt(1));


Count.Params param0 = control.count.makeParams(32).withMax(wordsPerBurst).withInc(1).withEnable(Enable);
Counter counter0 = control.count.makeCounter(param0);
HWVar wordCount = counter0.getCount();
HWVar wrap = (wordCount.eq((wordsPerBurst - 1)) & Enable);
Count.Params param1 = control.count.makeParams(32).withMax(totalBursts).withInc(1).withEnable(wrap);
Counter counter1 = control.count.makeCounter(param1);
HWVar burstCount = counter1.getCount();
HWVar wrap2 = (burstCount.eq((totalBursts - 1)) & wrap);
Count.Params param2 = control.count.makeParams(32).withMax(iterations).withInc(1).withEnable(wrap2);
Counter counter2 = control.count.makeCounter(param2);
HWVar iterCount = counter2.getCount();
HWVar Control = (wordCount.eq(0) & Enable);
DRAMCommandStream.makeKernelOutput("dram_write",
Control,
(burstCount + iniBursts),
constant.var(hwUInt(8),1),
constant.var(hwUInt(6),1),
constant.var(hwUInt(4),0),
(burstCount.eq((totalBursts - 1)) & iterCount.eq((iterations - 1))));

}
}
