
package MyApp_pkg;

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

public class MyApp1 extends Kernel {
  int Par=6;
  int Mul=1;
  int Sub=0; 
  
  public KArrayType<HWVar> burst_in=
    new KArrayType<HWVar>(hwFloat(8, 24),4*Par);
  
  public KArrayType<HWVar> Ring=
    new KArrayType<HWVar>(hwFloat(8, 24),Par);

  public KArrayType<HWVar> image_type=
    new KArrayType<HWVar>(hwFloat(8, 24), 8);
  
  public MyApp1 (KernelParameters parameters, int Diff) {
    super(parameters);
 
//------------------------------Parameter configuration --------------------------------
    OffsetExpr nx = stream.makeOffsetParam("nx",  24/Par, 24/Par);
    OffsetExpr nxy = stream.makeOffsetParam("nxy",32* nx, 32 * nx);

    HWVar c_0   = constant.var(-0.000000056202665632554272);
    HWVar c_1_0 = constant.var( 0.000000010666666661052204);
    HWVar c_1_1 = constant.var(-0.000000001523809300962853);
    HWVar c_1_2 = constant.var( 0.000000000253968179819708);
    HWVar c_1_3 = constant.var(-0.000000000031745984313547);
    HWVar c_1_4 = constant.var( 0.000000000002031744130576);
    HWVar c_2_0 = constant.var( 0.000000010666666661052204);
    HWVar c_2_1 = constant.var(-0.000000001523809300962853);
    HWVar c_2_2 = constant.var( 0.000000000253968179819708);
    HWVar c_2_3 = constant.var(-0.000000000031745984313547);
    HWVar c_2_4 = constant.var( 0.000000000002031744130576);
    HWVar c_3_0 = constant.var( 0.000000010666666661052204);
    HWVar c_3_1 = constant.var(-0.000000001523809300962853);
    HWVar c_3_2 = constant.var( 0.000000000253968179819708);
    HWVar c_3_3 = constant.var(-0.000000000031745984313547);
    HWVar c_3_4 = constant.var( 0.000000000002031744130576);

    HWVar bc    = constant.var(-0.0005);

    HWVar n1    = io.scalarInput("n1",    hwUInt(32));
    HWVar n2    = io.scalarInput("n2",    hwUInt(32));
    HWVar n3    = io.scalarInput("n3",    hwUInt(32));
    HWVar ORDER = io.scalarInput("ORDER", hwUInt(32));
    HWVar SPONGE= io.scalarInput("SPONGE",hwUInt(32));
      
//------------------------------Output control --------------------------------
    CounterChain chain = control.count.makeCounterChain();
    HWVar i4 = chain.addCounter(1000,1).cast(hwUInt(32));//iteration
    HWVar i3 = chain.addCounter(n3,  1).cast(hwUInt(32));//outest loop
    HWVar i2 = chain.addCounter(n2,  1).cast(hwUInt(32));
    HWVar i1 = chain.addCounter(n1,Par).cast(hwUInt(32));//innest loop
    
     
    HWVar up[] = new HWVar[Par];
    for (int i=0; i <Par; i++)
      up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;

    HWVar output_ring;
    output_ring= (Diff == 0) ? (n3 - ORDER > i3  & i3 >= n3 - 2 * ORDER) : (2 * ORDER > i3 & i3 >= ORDER);

    HWVar output_burst;
    output_burst= (n3 - ORDER > i3 & i3 >= ORDER);
//------------------------------Data input --------------------------------
    // input
    KArray<HWVar> burst_input  =io.input("burst_input", burst_in);

    HWVar p[]       =new HWVar[Par];
    HWVar pp_i[]    =new HWVar[Par];
    HWVar dvv[]     =new HWVar[Par];
    HWVar source[]  =new HWVar[Par];
    
    HWVar image[][] =new HWVar[Par][Mul];

    for (int i=0; i <Par; i++)
    {
            
      p[i]=       burst_input[Par*0+i].cast(hwFloat(8,24));
      pp_i[i]=    burst_input[Par*1+i].cast(hwFloat(8,24));
      dvv[i]=     burst_input[Par*2+i].cast(hwFloat(8,24));
      source[i]=  burst_input[Par*3+i].cast(hwFloat(8,24));
    }



//------------------------------Forward Data-path --------------------------------
    HWVar cur[][][][]    = new HWVar[Mul][11+Par+1][11][11];
    HWVar inter[][]      = new HWVar[Par][Mul];
    HWVar result[][]     = new HWVar[Par][Mul];
    
    // setup the optimisation factor to transfer operations between DSPs and Luts
    optimization.pushDSPFactor(1);
    //Cache
    for (int i=0; i <Par; i++)
    {
      int k = -6/Par;
      for (int x=-6; x<=6; x+=Par)
      {
        for (int y=-5; y<=5; y++)
          for (int z=-5; z<=5; z++)
            cur[0][x+6+i][y+5][z+5] = stream.offset(p[i], z*nxy+y*nx+k);
        k++;
      }
    }
    //Computation
    for (int i=0; i <Par; i++)
    {
    //data-path(0,i)      
    result[i][0]=(  
                    cur[0][6+i][5][5] * 2.0 - pp_i[i] +dvv[i]*(
                    cur[0][6+i][5][5] * c_0
                  +(cur[0][5+i][5][5] + cur[0][7+i][5][5]) * c_1_0
                  +(cur[0][4+i][5][5] + cur[0][8+i][5][5]) * c_1_1
                  +(cur[0][3+i][5][5] + cur[0][9+i][5][5]) * c_1_2
                  +(cur[0][2+i][5][5] + cur[0][10+i][5][5])* c_1_3
                  +(cur[0][1+i][5][5] + cur[0][11+i][5][5])* c_1_4
                  +(cur[0][6+i][4][5] + cur[0][6+i][6][5]) * c_2_0
                  +(cur[0][6+i][3][5] + cur[0][6+i][7][5]) * c_2_1
                  +(cur[0][6+i][2][5] + cur[0][6+i][8][5]) * c_2_2
                  +(cur[0][6+i][1][5] + cur[0][6+i][9][5]) * c_2_3
                  +(cur[0][6+i][0][5] + cur[0][6+i][10][5])* c_2_4
                  +(cur[0][6+i][5][4] + cur[0][6+i][5][6]) * c_3_0
                  +(cur[0][6+i][5][3] + cur[0][6+i][5][7]) * c_3_1
                  +(cur[0][6+i][5][2] + cur[0][6+i][5][8]) * c_3_2
                  +(cur[0][6+i][5][1] + cur[0][6+i][5][9]) * c_3_3
                  +(cur[0][6+i][5][0] + cur[0][6+i][5][10])* c_3_4 ))
                  + source[i];
    inter[i][0]   =(up[i])? result[i][0] : pp_i[i];
    }
    //Multiple Time-Dimension 
    for (int j=1; j <Mul; j++)
    {
      //Cache
      for (int i=0; i <Par; i++)
      {
        int k = -6/Par;
        for (int x=-6; x<=6; x+=Par)
        {
          for (int y=-5; y<=5; y++)
            for (int z=-5; z<=5; z++)
              cur[j][x+6+i][y+5][z+5] = stream.offset(inter[i][j-1], z*nxy+y*nx+k);
          k++;
        }
      }

      //Computation
      for (int i=0; i <Par; i++)
      {
      //data-path(j,i)      
      result[i][j]=(  
                      cur[j][6+i][5][5] * 2.0 - cur[j-1][6+i][5][5] +dvv[i]*(
                      cur[j][6+i][5][5] * c_0
                    +(cur[j][5+i][5][5] + cur[j][7+i][5][5]) * c_1_0
                    +(cur[j][4+i][5][5] + cur[j][8+i][5][5]) * c_1_1
                    +(cur[j][3+i][5][5] + cur[j][9+i][5][5]) * c_1_2
                    +(cur[j][2+i][5][5] + cur[j][10+i][5][5]) * c_1_3
                    +(cur[j][1+i][5][5] + cur[j][11+i][5][5])* c_1_4
                    +(cur[j][6+i][4][5] + cur[j][6+i][6][5]) * c_2_0
                    +(cur[j][6+i][3][5] + cur[j][6+i][7][5]) * c_2_1
                    +(cur[j][6+i][2][5] + cur[j][6+i][8][5]) * c_2_2
                    +(cur[j][6+i][1][5] + cur[j][6+i][9][5]) * c_2_3
                    +(cur[j][6+i][0][5] + cur[j][6+i][10][5])* c_2_4
                    +(cur[j][6+i][5][4] + cur[j][6+i][5][6]) * c_3_0
                    +(cur[j][6+i][5][3] + cur[j][6+i][5][7]) * c_3_1
                    +(cur[j][6+i][5][2] + cur[j][6+i][5][8]) * c_3_2
                    +(cur[j][6+i][5][1] + cur[j][6+i][5][9]) * c_3_3
                    +(cur[j][6+i][5][0] + cur[j][6+i][5][10])* c_3_4 ))
                    + source[i];
      inter[i][j]   =(up[i])? result[i][j] : cur[j-1][6+i][5][5];
      }
    }

    //setup configuration
    optimization.popDSPFactor();
    
//------------------------------Data output --------------------------------
    
    // control counter 
    KArray<HWVar> burst_output = burst_in.newInstance(this);

    for (int i=0; i <Par; i++)
    {
      burst_output[Par*0+i] <==       inter[i][Mul-1].cast(hwFloat(8,24));
      burst_output[Par*1+i] <== cur[Mul-1][6+i][5][5].cast(hwFloat(8,24));
      burst_output[Par*2+i] <==                dvv[i].cast(hwFloat(8,24));
      burst_output[Par*3+i] <==             source[i].cast(hwFloat(8,24));
    }
    //ring_output = burst_output;

    io.output("burst_output", burst_output, burst_in, output_burst);
    io.output( "ring_output", burst_input,  burst_in, output_ring);
    
//------------------------------Debug --------------------------------
//  debug.printf("cycle:%d-%d-%d\n",i3,i2,i1);
//  debug.printf("it:%d\n",it);
//  debug.printf("it_buf:%.22f\n",it_buf);
//  debug.printf("power:%.22f\n",power);
//  debug.printf("scale:%.22f\n",scale);
//  debug.printf("ppb:%.22f\n",interb[0][Mul-2]);
//  debug.printf("ppb:%.22f\n",interb[0][Mul-1]);
//  
    debug.printf("cycle:%d-%d-%d-%d\n",i4,i3,i2,i1);
    debug.printf("ring %d:%d\n",Diff, output_ring);
    debug.printf("burst%d:%d\n",Diff, output_burst);
    debug.printf("up[0]:%d\n",up[0]);
    debug.printf("p[0]:%.28f\n",   p[0]);
    debug.printf("pp_i[0]:%.28f\n",pp_i[0]);
    debug.printf("dvv[0]:%.28f\n", dvv[0]);
    debug.printf("source[0]:%.28f\n",source[0]);
    debug.printf("result[0]%d:%.28f\n",Diff,inter[0][Mul-1]);
    debug.printf("up[1]:%d\n",up[1]);
    debug.printf("p[1]:%.28f\n",   p[1]);
    debug.printf("pp_i[1]:%.28f\n",pp_i[1]);
    debug.printf("dvv[1]:%.28f\n", dvv[1]);
    debug.printf("source[1]:%.28f\n",source[1]);
    debug.printf("result[1]%d:%.28f\n",Diff,inter[1][Mul-1]);
//  debug.printf("Output:\n");
//  debug.printf("pp[0]:%.22f\n",inter[0][Mul-1]);
//  debug.printf("pp[1]:%.22f\n",inter[1][Mul-1]);
//  debug.printf("p[0]:%.22f\n", cur[Mul-1][5][5][5]);
//  debug.printf("p[1]:%.22f\n", cur[Mul-1][6][5][5]);
//  debug.printf("dvv[0]:%.22f\n", dvv[0]);
//  debug.printf("dvv[1]:%.22f\n", dvv[1]);
//  debug.printf("source0:%.22f\n",source[0]);
//  debug.printf("cur[0][1][5][5]:%.22f\n", cur[0][1][5][5]);
//  debug.printf("cur[0][2][5][5]:%.22f\n", cur[0][2][5][5]);
//  debug.printf("cur[0][3][5][5]:%.22f\n", cur[0][3][5][5]);
//  debug.printf("cur[0][4][5][5]:%.22f\n", cur[0][4][5][5]);
//  debug.printf("cur[0][5][5][5]:%.22f\n", cur[0][5][5][5]);
//  debug.printf("cur[0][6][5][5]:%.22f\n", cur[0][6][5][5]);
//  debug.printf("cur[0][7][5][5]:%.22f\n", cur[0][7][5][5]);
//  debug.printf("cur[0][8][5][5]:%.22f\n", cur[0][8][5][5]);
//  debug.printf("cur[0][9][5][5]:%.22f\n", cur[0][9][5][5]);
//  debug.printf("cur[0][10][5][5]:%.22f\n",cur[0][10][5][5]);
//  debug.printf("cur[0][11][5][5]:%.22f\n\n",cur[0][11][5][5]);

//  debug.printf("cur[0][2][5][5]:%.22f\n", cur[0][2][5][5]);
//  debug.printf("cur[0][3][5][5]:%.22f\n", cur[0][3][5][5]);
//  debug.printf("cur[0][4][5][5]:%.22f\n", cur[0][4][5][5]);
//  debug.printf("cur[0][5][5][5]:%.22f\n", cur[0][5][5][5]);
//  debug.printf("cur[0][6][5][5]:%.22f\n", cur[0][6][5][5]);
//  debug.printf("cur[0][7][5][5]:%.22f\n", cur[0][7][5][5]);
//  debug.printf("cur[0][8][5][5]:%.22f\n", cur[0][8][5][5]);
//  debug.printf("cur[0][9][5][5]:%.22f\n", cur[0][9][5][5]);
//  debug.printf("cur[0][10][5][5]:%.22f\n", cur[0][10][5][5]);
//  debug.printf("cur[0][11][5][5]:%.22f\n",cur[0][11][5][5]);
//  debug.printf("cur[0][12][5][5]:%.22f\n\n",cur[0][12][5][5]);
//  debug.printf("cur[0][6][0][5]:%.22f\n", cur[0][7][0][5]);
//  debug.printf("cur[0][6][1][5]:%.22f\n", cur[0][7][1][5]);
//  debug.printf("cur[0][6][2][5]:%.22f\n", cur[0][7][2][5]);
//  debug.printf("cur[0][6][3][5]:%.22f\n", cur[0][7][3][5]);
//  debug.printf("cur[0][6][4][5]:%.22f\n", cur[0][7][4][5]);
//  debug.printf("cur[0][6][5][5]:%.22f\n", cur[0][7][5][5]);
//  debug.printf("cur[0][6][6][5]:%.22f\n", cur[0][7][6][5]);
//  debug.printf("cur[0][6][7][5]:%.22f\n", cur[0][7][7][5]);
//  debug.printf("cur[0][6][8][5]:%.22f\n", cur[0][7][8][5]);
//  debug.printf("cur[0][6][9][5]:%.22f\n", cur[0][7][9][5]);
//  debug.printf("cur[0][6][10][5]:%.22f\n\n", cur[0][7][10][5]);
//  
//  debug.printf("cur[0][6][5][0]:%.22f\n", cur[0][7][5][0]);
//  debug.printf("cur[0][6][5][1]:%.22f\n", cur[0][7][5][1]);
//  debug.printf("cur[0][6][5][2]:%.22f\n", cur[0][7][5][2]);
//  debug.printf("cur[0][6][5][3]:%.22f\n", cur[0][7][5][3]);
//  debug.printf("cur[0][6][5][4]:%.22f\n", cur[0][7][5][4]);
//  debug.printf("cur[0][6][5][5]:%.22f\n", cur[0][7][5][5]);
//  debug.printf("cur[0][6][5][6]:%.22f\n", cur[0][7][5][6]);
//  debug.printf("cur[0][6][5][7]:%.22f\n", cur[0][7][5][7]);
//  debug.printf("cur[0][6][5][8]:%.22f\n", cur[0][7][5][8]);
//  debug.printf("cur[0][6][5][9]:%.22f\n", cur[0][7][5][9]);
//  debug.printf("cur[0][6][5][10]:%.22f\n\n", cur[0][7][5][10]);
  }
}
