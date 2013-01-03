package engine;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.CounterChain;


public class KernelFunc extends Kernel {

public KernelFunc(KernelParameters parameters) {
super(parameters);
HWVar x =  io.input("x", hwFloat(8, 24));
HWVar c =  io.scalarInput("c", hwFloat(8, 24));
HWVar a1 =  io.scalarInput("a1", hwUInt(32));
HWVar a2 =  io.scalarInput("a2", hwUInt(32));
HWVar a3 =  io.scalarInput("a3", hwUInt(32));
HWVar n1 =  io.scalarInput("n1", hwUInt(32));
HWVar n2 =  io.scalarInput("n2", hwUInt(32));
HWVar n3 =  io.scalarInput("n3", hwUInt(32));
HWVar nx =  io.scalarInput("nx", hwUInt(32));
HWVar ny =  io.scalarInput("ny", hwUInt(32));
HWVar nz =  io.scalarInput("nz", hwUInt(32));
CounterChain chain_i = control.count.makeCounterChain();


HWVar i = chain_i.addCounter(nz, 1);
HWVar j = chain_i.addCounter(ny, 1);
HWVar k = chain_i.addCounter(nx, 1);
HWVar cond = ((((i >= a1) & (i < n1)) & ((j >= a2) & (j < n2))) & ((k >= a3) & (k < n3)));
HWVar func = (((((((c * stream.offset(x, 0)) + (c * stream.offset(x, 1))) + (c * stream.offset(x, -constant.var(1), -1024, 1024))) + (c * stream.offset(x, -nx, -1024, 1024))) + (c * stream.offset(x, nx, -1024, 1024))) + (c * stream.offset(x, (-nx * ny), -1024, 1024))) + (c * stream.offset(x, (nx * ny), -1024, 1024)));
io.output("res", func, hwFloat(8, 24), cond);
}
}
