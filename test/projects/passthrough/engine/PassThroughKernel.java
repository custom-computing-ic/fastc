package engine;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;

public class PassThroughKernel extends Kernel {

  public PassThroughKernel(KernelParameters parameters) {
    super(parameters);

    HWVar x = io.input("x",  hwFloat(8, 24));
    io.output("y", x, hwFloat(8, 24));
  }
}
