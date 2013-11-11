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

public class Convolution1d extends Kernel {


public Convolution1d(KernelParameters parameters) {
super(parameters);
HWVar p =  io.input("p", hwFloat(8, 24));
HWVar c_0_0_0 =  io.scalarInput("c_0_0_0", hwFloat(8, 24));
HWVar c_p_0_0 =  io.scalarInput("c_p_0_0", hwFloat(8, 24));
HWVar c_n_0_0 =  io.scalarInput("c_n_0_0", hwFloat(8, 24));
HWVar n1 =  io.scalarInput("n1", hwUInt(32));
HWVar ORDER =  io.scalarInput("ORDER", hwUInt(32));

Count.Params param0 = control.count.makeParams(32).withMax(n1).withInc(1);
Counter i1_counter = control.count.makeCounter(param0);
HWVar i1 = i1_counter.getCount();
Count.Params param1 = control.count.makeParams(10).withMax(1000).withInc(1).withEnable(i1_counter.getWrap());
Counter i4_counter = control.count.makeCounter(param1);
HWVar i4 = i4_counter.getCount();
HWVar up = ((i1 >= ORDER) & (i1 < (n1 - ORDER)));
optimization.pushDSPFactor(1);
HWVar result;
result = (((p * c_0_0_0) + (stream.offset(p, 1) * c_p_0_0)) + (stream.offset(p, -1) * c_n_0_0));
HWVar inter = up.neq(0)?result:p;
optimization.popDSPFactor();

io.output("output", inter, hwFloat(8, 24));
}}