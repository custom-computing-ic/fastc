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

public class CmdWrite extends Kernel {


public CmdWrite(KernelParameters parameters) {
super(parameters);
HWVar iniBursts =  io.scalarInput("iniBursts", hwUInt(32));
HWVar iterations =  io.scalarInput("iterations", hwUInt(32));
HWVar totalBursts =  io.scalarInput("totalBursts", hwUInt(32));
HWVar wordsPerBurst =  io.scalarInput("wordsPerBurst", hwUInt(32));
HWVar Enable =  io.scalarInput("Enable", hwUInt(1));

Count.Params param0 = control.count.makeParams(32).withMax(wordsPerBurst).withInc(1).withEnable(Enable);
Counter wordCount_counter = control.count.makeCounter(param0);
HWVar wordCount = wordCount_counter.getCount();
Count.Params param1 = control.count.makeParams(32).withMax(totalBursts).withInc(1).withEnable(wordCount_counter.getWrap());
Counter burstCount_counter = control.count.makeCounter(param1);
HWVar burstCount = burstCount_counter.getCount();
Count.Params param2 = control.count.makeParams(32).withMax(iterations).withInc(1).withEnable(burstCount_counter.getWrap());
Counter iterCount_counter = control.count.makeCounter(param2);
HWVar iterCount = iterCount_counter.getCount();
HWVar Control = (wordCount.eq(0) & Enable);
DRAMCommandStream.makeKernelOutput("dram_write",
Control,
(burstCount + iniBursts),
constant.var(hwUInt(8),1),
constant.var(hwUInt(6),1),
constant.var(hwUInt(4),0),
(burstCount.eq((totalBursts - 1)) & iterCount.eq((iterations - 1))));

}}