
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
import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWFloat;
import com.maxeler.maxcompiler.v1.kernelcompiler.RoundingMode;

public class MyApp extends Kernel {
  int Par=1;
  int Mul=1;
  int Sub=0; 
  
  public KArrayType<HWVar> burst_p=
    new KArrayType<HWVar>(hwFloat(8, 24),  Par*2);
  public KArrayType<HWVar> burst_in=
    new KArrayType<HWVar>(hwFloat(8, 24),  Par*6);
  public KArrayType<HWVar> burst_out=
    new KArrayType<HWVar>(hwFloat(8, 24),  Par*6);

  public MyApp (KernelParameters parameters) {
    super(parameters);
    HWFloat real    = hwFloat(8,24);
    HWFix   fix_4_24= hwFix(4,24,HWFix.SignMode.TWOSCOMPLEMENT);
 
//------------------------------Parameter configuration --------------------------------
    OffsetExpr nx = stream.makeOffsetParam("nx",  24/Par, 48/Par);
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
    output_ring= n3  > i3  & i3 >= n3 - ORDER;
    //output_ring= n3 - ORDER > i3  & i3 >= n3 - 2 * ORDER;
      
    HWVar input_ring;
    input_ring = ORDER > i3;
    //input_ring = 2 * ORDER > i3 & i3 >= ORDER;

    HWVar output_enable;
    output_enable= (n3 - ORDER > i3 & i3 >= ORDER);
//------------------------------Data input --------------------------------
    // input
    KArray<HWVar> input_p      =io.input("burst_p",       burst_p);
    KArray<HWVar> burst_input  =io.input("burst_input",   burst_in);

    HWVar p[]       =new HWVar[Par];
    HWVar pp_i[]    =new HWVar[Par];
    HWVar dvv[]     =new HWVar[Par];
    HWVar source[]  =new HWVar[Par];

    HWVar pb[]      =new HWVar[Par];
    HWVar ppb_i[]   =new HWVar[Par];
    HWVar dvvb[]    =new HWVar[Par];
    HWVar sourceb[] =new HWVar[Par];
    
    HWVar image[][] =new HWVar[Par][Mul];

    for (int i=0; i <Par; i++)
    {
      p[i]=       input_p[Par*0+i].cast(real);
      pb[i]=      input_p[Par*1+i].cast(real);
    }
    
    for (int i=0; i <Par; i++)
    {
      pp_i[i]=    burst_input[Par*0+i].cast(real);
      dvv[i]=     burst_input[Par*1+i].cast(real);
      source[i]=  burst_input[Par*2+i].cast(real);
      ppb_i[i]=   burst_input[Par*3+i].cast(real);
      dvvb[i]=    burst_input[Par*4+i].cast(real);
      image[i][0]=burst_input[Par*5+i].cast(real);
    }

//------------------------------boundary control--------------------------
    HWVar f3 =(i3 <  SPONGE       )? (SPONGE - i3) :
      ((i3 >= (n3 - SPONGE))? (i3-(n3-SPONGE)+1) : 0);
    HWVar f3s= (f3 * f3);
    HWVar f2 = i2 <  SPONGE        ? (SPONGE - i2) :
      ((i2 >= (n2 - SPONGE))? (i2-(n2-SPONGE)+1) : 0);
    HWVar f2s= (f2 * f2);
    HWVar f1 = i1*Par <  SPONGE        ? (SPONGE - i1*Par) :
      ((i1 >= (n1 - SPONGE))? (i1*Par-(n1-SPONGE)+1) : 0);
    HWVar f1s= (f1 * f1);
    HWVar is = f1s + f2s + f3s;

    optimization.pushRoundingMode(RoundingMode.TRUNCATE);

    KernelMath.Range range = new KernelMath.Range(0.0, 20.0);
    HWVar it =  KernelMath.sqrt(range, is, hwUInt(32));

    optimization.popRoundingMode();

    HWVar it_buf  = (it  < SPONGE) ? it : SPONGE;
    it_buf  =  it_buf.cast(hwFloat(8,24));

    HWVar power = bc * it_buf;
    HWVar pow   =(power).cast(hwFix(2, 30, HWFix.SignMode.TWOSCOMPLEMENT));

    KernelMath.Range powrange = new KernelMath.Range(-0.1, 0.0);
    HWVar scale = (it > 0) ? KernelMath.exp(pow) : 1;
    scale    = scale.cast(hwFloat(8,24));


//------------------------------Forward Data-path ------------------------
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
                  +(cur[0][6+i][5][0] + cur[0][6+i][5][10])* c_3_4 ));
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
                    +(cur[j][6+i][5][0] + cur[j][6+i][5][10])* c_3_4 ));
      inter[i][j]   =(up[i])? result[i][j] : cur[j-1][6+i][5][5];
      }
    }

    //setup configuration
    optimization.popDSPFactor();

//------------------------------Backward Data-path --------------------------------
    HWVar curb[][][][] = new HWVar[Mul][11+Par+1][11][11];
    HWVar interb[][]   = new HWVar[Par][Mul];
    HWVar resultb[][]  = new HWVar[Par][Mul];
    
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
           curb[0][x+6+i][y+5][z+5] = stream.offset(pb[i], z*nxy+y*nx+k);
        k++;
      }
    }

    //Computation
    for (int i=0; i <Par; i++)
    {
    //data-pathi     
    ppb_i[i]     = ppb_i[i] * scale; 
    resultb[i][0]=( 
                    curb[0][6+i][5][5] * 2.0 - ppb_i[i] +dvvb[i]*(
                    curb[0][6+i][5][5] * c_0
                  +(curb[0][5+i][5][5] + curb[0][7+i][5][5]) * c_1_0
                  +(curb[0][4+i][5][5] + curb[0][8+i][5][5]) * c_1_1
                  +(curb[0][3+i][5][5] + curb[0][9+i][5][5]) * c_1_2
                  +(curb[0][2+i][5][5] + curb[0][10+i][5][5])* c_1_3
                  +(curb[0][1+i][5][5] + curb[0][11+i][5][5])* c_1_4
                  +(curb[0][6+i][4][5] + curb[0][6+i][6][5]) * c_2_0
                  +(curb[0][6+i][3][5] + curb[0][6+i][7][5]) * c_2_1
                  +(curb[0][6+i][2][5] + curb[0][6+i][8][5]) * c_2_2
                  +(curb[0][6+i][1][5] + curb[0][6+i][9][5]) * c_2_3
                  +(curb[0][6+i][0][5] + curb[0][6+i][10][5])* c_2_4
                  +(curb[0][6+i][5][4] + curb[0][6+i][5][6]) * c_3_0
                  +(curb[0][6+i][5][3] + curb[0][6+i][5][7]) * c_3_1
                  +(curb[0][6+i][5][2] + curb[0][6+i][5][8]) * c_3_2
                  +(curb[0][6+i][5][1] + curb[0][6+i][5][9]) * c_3_3
                  +(curb[0][6+i][5][0] + curb[0][6+i][5][10])* c_3_4 ))
                  + source[i];
    resultb[i][0] = resultb[i][0] * scale;
    interb[i][0]  = (up[i]) ? resultb[i][0] : ppb_i[i];
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
             curb[j][x+6+i][y+5][z+5] = stream.offset(interb[i][j-1], z*nxy+y*nx+k);
          k++;
        }
      }
      
      for (int i=0; i <Par; i++)
      {
      //data-pathi     
      curb[j-1][6+i][5][5] = curb[j-1][6+i][5][5] * scale;
      resultb[i][j]=( 
                      curb[j][6+i][5][5] * 2.0 - curb[j-1][5+i][5][5] +dvvb[i]*(
                      curb[j][6+i][5][5] * c_0
                    +(curb[j][5+i][5][5] + curb[j][7+i][5][5]) * c_1_0
                    +(curb[j][4+i][5][5] + curb[j][8+i][5][5]) * c_1_1
                    +(curb[j][3+i][5][5] + curb[j][9+i][5][5]) * c_1_2
                    +(curb[j][2+i][5][5] + curb[j][10+i][5][5]) * c_1_3
                    +(curb[j][1+i][5][5] + curb[j][11+i][5][5])* c_1_4
                    +(curb[j][6+i][4][5] + curb[j][6+i][6][5]) * c_2_0
                    +(curb[j][6+i][3][5] + curb[j][6+i][7][5]) * c_2_1
                    +(curb[j][6+i][2][5] + curb[j][6+i][8][5]) * c_2_2
                    +(curb[j][6+i][1][5] + curb[j][6+i][9][5]) * c_2_3
                    +(curb[j][6+i][0][5] + curb[j][6+i][10][5])* c_2_4
                    +(curb[j][6+i][5][4] + curb[j][6+i][5][6]) * c_3_0
                    +(curb[j][6+i][5][3] + curb[j][6+i][5][7]) * c_3_1
                    +(curb[j][6+i][5][2] + curb[j][6+i][5][8]) * c_3_2
                    +(curb[j][6+i][5][1] + curb[j][6+i][5][9]) * c_3_3
                    +(curb[j][6+i][5][0] + curb[j][6+i][5][10])* c_3_4 ))
                    + source[i];
      resultb[i][j] = resultb[i][j] * scale;
      interb[i][j]  =(up[i])? resultb[i][j] : curb[j-1][6+i][5][5];
      }
    }

    //setup configuration
    optimization.popDSPFactor();

//------------------------------Imaging --------------------------------
    for (int j =0; j< Mul-1; j++)
      for (int i = 0; i < Par; i++)
        image[i][j+1]  = image[i][j]     + inter[i][j]     * interb[i][j];
        
    debug.printf("image:%.28f\n\n",image[0][Mul-1]);
    for (int i = 0; i < Par; i++)
      image[i][Mul-1]  = image[i][Mul-1] + inter[i][Mul-1] * interb[i][Mul-1];

//------------------------------Data output --------------------------------
    
    // control counter 
    KArray<HWVar> output_p  = burst_p.newInstance(this);
    KArray<HWVar> burst_output = burst_out.newInstance(this);

    for (int i=0; i <Par; i++)
    {
      output_p[Par*0+i]  <==       inter[i][Mul-1].cast(hwFloat(8,24));
      output_p[Par*1+i]  <==       interb[i][Mul-1].cast(hwFloat(8,24));
    }
    
    for (int i=0; i <Par; i++)
    {
      burst_output[Par*0+i] <== cur[Mul-1][6+i][5][5].cast(hwFloat(8,24));
      burst_output[Par*1+i] <== dvv[i].cast(hwFloat(8,24));
      burst_output[Par*2+i] <== source[i].cast(hwFloat(8,24));
      burst_output[Par*3+i] <== curb[Mul-1][6+i][5][5].cast(hwFloat(8,24));
      burst_output[Par*4+i] <== dvvb[i].cast(hwFloat(8,24));
      burst_output[Par*5+i] <== image[i][Mul-1].cast(hwFloat(8,24));
    }


    io.output("output_p",     output_p,     burst_p);
    io.output("output_pp",    burst_output, burst_out);
    
//------------------------------Debug --------------------------------
    debug.printf("cycle:%d-%d-%d-%d\n",i4,i3,i2,i1);
    debug.printf("up[0]:%d\n",up[0]);
    debug.printf("p[0]:%.28f\n",   p[0]);
    debug.printf("pp_i[0]:%.28f\n",pp_i[0]);
    debug.printf("dvv[0]:%.28f\n", dvv[0]);
    debug.printf("source[0]:%.28f\n",source[0]);
    debug.printf("result[0]:%.28f\n\n",inter[0][Mul-1]);
    debug.printf("pb[0]:%.28f\n",   pb[0]);
    debug.printf("ppb_i[0]:%.28f\n",ppb_i[0]);
    debug.printf("dvvb[0]:%.28f\n", dvvb[0]);
    debug.printf("resultb[0]:%.28f\n",interb[0][Mul-1]);
    debug.printf("image:%.28f\n\n",image[0][Mul-1]);
  }
}
