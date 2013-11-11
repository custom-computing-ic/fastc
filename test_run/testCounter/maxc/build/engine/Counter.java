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

Count.Params param0 = control.count.makeParams(7).withMax(100).withInc(1);
Counter z_counter = control.count.makeCounter(param0);
HWVar z = z_counter.getCount();
Count.Params param1 = control.count.makeParams(7).withMax(100).withInc(1).withEnable(z_counter.getWrap());
Counter y_counter = control.count.makeCounter(param1);
HWVar y = y_counter.getCount();
HWVar enable = z.eq(y);
Count.Params param2 = control.count.makeParams(7).withMax(100).withInc(1).withEnable(enable);
Counter equals_counter = control.count.makeCounter(param2);
HWVar equals = equals_counter.getCount();

io.output("out", (equals + 1), hwInt(32));
}}