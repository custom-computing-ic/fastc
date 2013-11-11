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

public class Counter extends Kernel {


public Counter(KernelParameters parameters) {
super(parameters);

CounterChain chain_count1 = control.count.makeCounterChain();
CounterChain chain_count31 = control.count.makeCounterChain();

HWVar count1 = chain_count1.addCounter(1000, 1);
HWVar count2 = chain_count1.addCounter(10, 1);
HWVar count22 = chain_count1.addCounter(20, 2);
Count.Params param0 = control.count.makeParams(32).withMax(5).withInc(1).withEnable(count1);
Counter counter0 = control.count.makeCounter(param0);
HWVar count3 = counter0.getCount();
HWVar enable = count3.eq(count2);
Count.Params param1 = control.count.makeParams(16).withMax(10).withInc(2).withEnable(enable);
Counter counter1 = control.count.makeCounter(param1);
HWVar count4 = counter1.getCount();
HWVar count31 = chain_count31.addCounter(1000, 1);
HWVar count32 = chain_count31.addCounter(10, 1);
HWVar count322 = chain_count31.addCounter(20, 2);

}}