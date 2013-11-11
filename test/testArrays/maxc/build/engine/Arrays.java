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

public class Arrays extends Kernel {


public Arrays(KernelParameters parameters) {
super(parameters);
KArray<HWVar> array_in = io.input("array_in",  new KArrayType<HWVar>(hwFloat(8, 24),Par));
KArray<HWVar> array_out_1 = (new KArrayType<HWVar>(hwFloat(8, 24),Par)).newInstance(this);
KArray<HWVar> array_out_2 = (new KArrayType<HWVar>(hwFloat(8, 24),Par)).newInstance(this);

HWVar hw_up[] = new HWVar [3];
hw_up[0] = 1;
hw_up[1] = 2;
hw_up[2] = 3;
HWVar hw_up3d[][][] = new HWVar [1][1][1];
hw_up3d[0][0][0] = ((hw_up[0] + hw_up[1]) + hw_up[2]);
HWVar p[] = new HWVar [2];
p[0] = ((p[1] + stream.offset(p[1], 2)) + hw_up3d[0][0][0]);
HWVar pp[][] = new HWVar [2][4];
pp[0][1] = ((stream.offset(pp[1][0], 3) + pp[0][1]) + stream.offset(pp[0][1], 2));
pp[0][2] = ((stream.offset(pp[1][0], 3) + pp[0][1]) + stream.offset(pp[0][1], 2));
for (int i = 0;i < 2;i++) {
array_out_1[i] <== pp[0][i];
array_out_2[i] <== pp[0][i];
}

io.output("array_out_1", array_out_1, new KArrayType<HWVar> (hwFloat(8, 24), Par));
io.output("array_out_2", array_out_2, new KArrayType<HWVar> (hwFloat(8, 24), Par));
}}