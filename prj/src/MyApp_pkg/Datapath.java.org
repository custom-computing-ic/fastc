
package MyApp_pkg;

import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.DRAMCommandStream;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.CounterChain;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.KArray;
import com.maxeler.maxcompiler.v1.kernelcompiler.types.composite.KArrayType;
import com.maxeler.maxcompiler.v1.kernelcompiler.stdlib.core.Stream.OffsetExpr;
import com.maxeler.maxcompiler.v1.kernelcompiler.KernelLib;


public class Datapath extends KernelLib {
  
  public HWVar p_res, pp_res;
  public Datapath (KernelLib owner,
                   OffsetExpr nx, OffsetExpr nxy,
                   HWVar c_0,
                   HWVar c_1_0,HWVar c_1_1,HWVar c_1_2,HWVar c_1_3,HWVar c_1_4,
                   HWVar c_2_0,HWVar c_2_1,HWVar c_2_2,HWVar c_2_3,HWVar c_2_4,
                   HWVar c_3_0,HWVar c_3_1,HWVar c_3_2,HWVar c_3_3,HWVar c_3_4,
                   HWVar n1,   HWVar n2,   HWVar n3,   HWVar ORDER,
                   HWVar p,    HWVar pp_i, HWVar dvv,  HWVar source) {
    super(owner);
    
    int Mul=1;  

//------------------------------The control part--------------------------------
    //This part is specific to every data-path, for synchronisation
    CounterChain chain = control.count.makeCounterChain();
    HWVar i3    = chain.addCounter(n3, 1).cast(hwUInt(32));//outest loop
    HWVar i2    = chain.addCounter(n2, 1).cast(hwUInt(32));
    HWVar i1    = chain.addCounter(n1, 1).cast(hwUInt(32));//innest loop
    HWVar up    = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER   & i1<n1-ORDER;//  <---------------pay attention, if Mul>1, need modifications here

//------------------------------Internal data defination--------------------------------
    HWVar cur[][][][] = new HWVar[Mul][11][11][11];
    HWVar inter[]     = new HWVar[Mul];
    HWVar result[]    = new HWVar[Mul];

//------------------------------The data-path part--------------------------------
    // Compute
    for (int x=-5; x<=5; x++)
      for (int y=-5; y<=5; y++)
        for (int z=-5; z<=5; z++)
          cur[0][x+5][y+5][z+5] = stream.offset(p, z*nxy+y*nx+x);
    //data-path
    result[0]     =(2.0*cur[0][5][5][5]  - pp_i +dvv*(
                    c_0*cur[0][5][5][5]
                    +(cur[0][4][5][5] + cur[0][6][5][5]) * c_1_0 
                    +(cur[0][3][5][5] + cur[0][7][5][5]) * c_1_1 
                    +(cur[0][2][5][5] + cur[0][8][5][5]) * c_1_2 
                    +(cur[0][1][5][5] + cur[0][9][5][5]) * c_1_3 
                    +(cur[0][0][5][5] + cur[0][10][5][5])* c_1_4
                    +(cur[0][5][4][5] + cur[0][5][6][5]) * c_2_0 
                    +(cur[0][5][3][5] + cur[0][5][7][5]) * c_2_1 
                    +(cur[0][5][2][5] + cur[0][5][8][5]) * c_2_2 
                    +(cur[0][5][1][5] + cur[0][5][9][5]) * c_2_3 
                    +(cur[0][5][0][5] + cur[0][5][10][5])* c_2_4
                    +(cur[0][5][5][4] + cur[0][5][5][6]) * c_3_0 
                    +(cur[0][5][5][3] + cur[0][5][5][7]) * c_3_1 
                    +(cur[0][5][5][2] + cur[0][5][5][8]) * c_3_2 
                    +(cur[0][5][5][1] + cur[0][5][5][9]) * c_3_3 
                    +(cur[0][5][5][0] + cur[0][5][5][10])* c_3_4 )) 
                    + source;
    //multiplex
    inter[0]       = (up[0])? result[0] : pp_i;      
   
   for (int i=1;i<Mul;i++)
   { 
    //cube buffer   
    for (int x=-5; x<=5; x++)
      for (int y=-5; y<=5; y++)
        for (int z=-5; z<=5; z++)
          cur[i][x+5][y+5][z+5] = stream.offset(inter[i-1], z*nxy+y*nx+x);
    
    //data-path
    result[i]     =(2.0*cur[i][5][5][5] - cur[i-1][5][5][5]+dvv*(
                    c_0*cur[i][5][5][5]
                    +(cur[i][4][5][5] + cur[i][6][5][5]) * c_1_0 
                    +(cur[i][3][5][5] + cur[i][7][5][5]) * c_1_1 
                    +(cur[i][2][5][5] + cur[i][8][5][5]) * c_1_2 
                    +(cur[i][1][5][5] + cur[i][9][5][5]) * c_1_3 
                    +(cur[i][0][5][5] + cur[i][10][5][5])* c_1_4
                    +(cur[i][5][4][5] + cur[i][5][6][5]) * c_2_0 
                    +(cur[i][5][3][5] + cur[i][5][7][5]) * c_2_1 
                    +(cur[i][5][2][5] + cur[i][5][8][5]) * c_2_2 
                    +(cur[i][5][1][5] + cur[i][5][9][5]) * c_2_3 
                    +(cur[i][5][0][5] + cur[i][5][10][5])* c_2_4
                    +(cur[i][5][5][4] + cur[i][5][5][6]) * c_3_0 
                    +(cur[i][5][5][3] + cur[i][5][5][7]) * c_3_1 
                    +(cur[i][5][5][2] + cur[i][5][5][8]) * c_3_2 
                    +(cur[i][5][5][1] + cur[i][5][5][9]) * c_3_3 
                    +(cur[i][5][5][0] + cur[i][5][5][10])* c_3_4 )) 
                    + source;
    
    // multiplex
    inter[i]       = (up[i])? result[i] : cur[i][5][5][5];      
    }

  p_res = inter[Mul-1];
  pp_res= cur[Mul-1][5][5][5];
  }
}
