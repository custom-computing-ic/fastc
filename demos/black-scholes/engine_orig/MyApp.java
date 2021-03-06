package engine_orig;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.CounterChain;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.KArray;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.KArrayType;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Stream.OffsetExpr;
//import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Constant;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.KernelMath;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.KernelMath.Range;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWFix;
import com.maxeler.maxcompiler.v1.kernelcompiler.RoundingMode;

public class MyApp extends Kernel {
  public MyApp (KernelParameters parameters, int Diff) {
    super(parameters);

//------------------------------Parameter configuration --------------------------------
    HWVar c_0_0_0 = io.scalarInput("c_0_0_0", hwFloat(8, 24));
    HWVar c_p_0_0 = io.scalarInput("c_p_0_0", hwFloat(8, 24));
    HWVar c_n_0_0 = io.scalarInput("c_n_0_0", hwFloat(8, 24));

    HWVar n1    = io.scalarInput("n1",    hwUInt(32));
    HWVar ORDER = io.scalarInput("ORDER", hwUInt(32));

//------------------------------Output control --------------------------------
    CounterChain chain = control.count.makeCounterChain();
    HWVar i4 = chain.addCounter(1000,1).cast(hwUInt(32));//iteration
    HWVar i1 = chain.addCounter(n1,1).cast(hwUInt(32));//innest loop

    HWVar up;
    up = i1>=ORDER  & i1<n1-ORDER;
//------------------------------Data input --------------------------------
    // input
    HWVar p = io.input("input", hwFloat(8,24));

//------------------------------Forward Data-path --------------------------------
    HWVar cur[]      = new HWVar[3];
    HWVar inter;
    HWVar result;

    // setup the optimisation factor to transfer operations between DSPs and Luts
    optimization.pushDSPFactor(1);

    //Cache
    for (int x=-1; x<=1; x+=1)
      cur[x+1] = stream.offset(p, x);

    //Computation
    result=    cur[1] * c_0_0_0
             + cur[2] * c_p_0_0
             + cur[0] * c_n_0_0;
    inter   =up? result : p;

    //setup configuration
    optimization.popDSPFactor();

//------------------------------Data output --------------------------------

    // control counter
    io.output("output",  inter, hwFloat(8,24));

//------------------------------Debug --------------------------------
  }
}
