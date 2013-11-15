package MyApp_pkg;

import com.maxeler.maxcompiler.v1.kernelcompiler.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.*    ;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.KernelMath.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Stream.OffsetExpr;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.*;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.*;

public class do_forward extends Kernel {


public do_forward(KernelParameters parameters) {
super(parameters);
HWVar dvv    =  io.input("dvv", hwFloat(8, 24));
HWVar p      =  io.input("p", hwFloat(8, 24));
HWVar pp     =  io.input("pp", hwFloat(8, 24));
HWVar source =  io.input("source", hwFloat(8, 24));
HWVar n1     =  io.scalarInput("n1", hwInt(32));
HWVar n2     =  io.scalarInput("n2", hwInt(32));
HWVar n3     =  io.scalarInput("n3", hwInt(32));
HWVar n12    =  io.scalarInput("n12", hwInt(32));
HWVar ORDER  =  io.scalarInput("ORDER", hwInt(32));
HWVar c_0 =  io.scalarInput("c_0", hwFloat(8, 24));
HWVar c_1_0 =  io.scalarInput("c_1_0", hwFloat(8, 24));
HWVar c_1_1 =  io.scalarInput("c_1_1", hwFloat(8, 24));
HWVar c_1_2 =  io.scalarInput("c_1_2", hwFloat(8, 24));
HWVar c_1_3 =  io.scalarInput("c_1_3", hwFloat(8, 24));
HWVar c_1_4 =  io.scalarInput("c_1_4", hwFloat(8, 24));
HWVar c_2_0 =  io.scalarInput("c_2_0", hwFloat(8, 24));
HWVar c_2_1 =  io.scalarInput("c_2_1", hwFloat(8, 24));
HWVar c_2_2 =  io.scalarInput("c_2_2", hwFloat(8, 24));
HWVar c_2_3 =  io.scalarInput("c_2_3", hwFloat(8, 24));
HWVar c_2_4 =  io.scalarInput("c_2_4", hwFloat(8, 24));
HWVar c_3_0 =  io.scalarInput("c_3_0", hwFloat(8, 24));
HWVar c_3_1 =  io.scalarInput("c_3_1", hwFloat(8, 24));
HWVar c_3_2 =  io.scalarInput("c_3_2", hwFloat(8, 24));
HWVar c_3_3 =  io.scalarInput("c_3_3", hwFloat(8, 24));
HWVar c_3_4 =  io.scalarInput("c_3_4", hwFloat(8, 24));



/*-----Output Control------*/
CounterChain chain = control.count.makeCounterChain();
HWVar i3 = chain.addCounter(128, 1).cast(hwInt(32));
HWVar i2 = chain.addCounter(128, 1).cast(hwInt(32));
HWVar i1 = chain.addCounter(128, 1).cast(hwInt(32));
HWVar update = i3 < (n3 - ORDER) & i3 >= ORDER & i2 < (n2 - ORDER) & i2 >= ORDER & i1 < (n1 - ORDER) & i1 >= ORDER;


/*-----DataPath------*/
HWVar pp_out = ((((2.0 * p) - pp) + source) + (dvv * ((((((((((((((((p * c_0) + (c_1_0 * (stream.offset(p,1*1+128*0+16384*0) + stream.offset(p,1*-1+128*0+16384*0)))) + (c_1_1 * (stream.offset(p,1*2+128*0+16384*0) + stream.offset(p,1*-2+128*0+16384*0)))) + (c_1_2 * (stream.offset(p,1*3+128*0+16384*0) + stream.offset(p,1*-3+128*0+16384*0)))) + (c_1_3 * (stream.offset(p,1*4+128*0+16384*0) + stream.offset(p,1*-4+128*0+16384*0)))) + (c_1_4 * (stream.offset(p,1*5+128*0+16384*0) + stream.offset(p,1*-5+128*0+16384*0)))) + (c_2_0 * (stream.offset(p,1*0+128*1+16384*0) + stream.offset(p,1*0+128*-1+16384*0)))) + (c_2_1 * (stream.offset(p,1*0+128*2+16384*0) + stream.offset(p,1*0+128*-2+16384*0)))) + (c_2_2 * (stream.offset(p,1*0+128*3+16384*0) + stream.offset(p,1*0+128*-3+16384*0)))) + (c_2_3 * (stream.offset(p,1*0+128*4+16384*0) + stream.offset(p,1*0+128*-4+16384*0)))) + (c_2_4 * (stream.offset(p,1*0+128*5+16384*0) + stream.offset(p,1*0+128*-5+16384*0)))) + (c_3_0 * (stream.offset(p,1*0+128*0+16384*1) + stream.offset(p,1*0+128*0+16384*-1)))) + (c_3_1 * (stream.offset(p,1*0+128*0+16384*2) + stream.offset(p,1*0+128*0+16384*-2)))) + (c_3_2 * (stream.offset(p,1*0+128*0+16384*3) + stream.offset(p,1*0+128*0+16384*-3)))) + (c_3_3 * (stream.offset(p,1*0+128*0+16384*4) + stream.offset(p,1*0+128*0+16384*-4)))) + (c_3_4 * (stream.offset(p,1*0+128*0+16384*5) + stream.offset(p,1*0+128*0+16384*-5))))));
HWVar p_out = pp;


/*-----Outputs------*/
io.output( "pp_out", update ? pp_out : p , hwFloat(8, 24));
io.output( "p_out",p_out,hwFloat(8, 24));



}}
