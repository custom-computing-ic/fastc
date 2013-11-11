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

public class PassThroughKernel extends Kernel {


public PassThroughKernel(KernelParameters parameters) {
super(parameters);
HWVar x =  io.input("x", hwInt(32));


io.output("y", x, hwInt(32));
}}