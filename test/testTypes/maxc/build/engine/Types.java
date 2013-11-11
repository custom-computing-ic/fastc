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

public class Types extends Kernel {


public Types(KernelParameters parameters) {
super(parameters);
HWVar sin1 =  io.input("sin1", hwFloat(8, 24));

HWVar svar1 = sin1;
HWVar var1 = constant.var(0.335);
HWVar const1 = constant.var(0.5);
HWVar inter = (sin1 * var1);
HWVar res;
res = inter;

io.output("sout1", res, hwFloat(8, 24));
}}