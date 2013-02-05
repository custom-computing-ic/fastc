#include "../../../include/maxcc.h"
#include "../include/params_dse.h"


// XXX need to also evaluate Par before evaluating macro


#define burst_in  8, 24, 6
#define t_burst_p 8, 24, 2
#define burst_out 8, 24, 6

#pragma class:scalar dir:in name:n1 type:uint32 func:kernel_MyApp
#pragma class:scalar dir:in name:n2 type:uint32 func:kernel_MyApp
#pragma class:scalar dir:in name:n3 type:uint32 func:kernel_MyApp
#pragma class:scalar dir:in name:ORDER type:uint32 func:kernel_MyApp
#pragma class:scalar dir:in name:SPONGE type:uint32 func:kernel_MyApp
//#pragma class:array  dir:in name:burst_p type:float8_24 width:1 func:kernel_MyApp
//#pragma class:array  dir:in name:burst_pp type:float8_24 width:1 func:kernel_MyApp
//#pragma class:array  dir:in name:burst_dvv type:float8_24 width:1 func:kernel_MyApp
//#pragma class:array  dir:out name:output_p type:float8_24 width:1 func:kernel_MyApp
//#pragma class:array  dir:out name:output_pp type:float8_24 width:1 func:kernel_MyApp
void kernel_MyApp(
                  uint32 n1, uint32 n2, uint32 n3,
                  uint32 ORDER, uint32 SPONGE,
                  s_array_f8_24 burst_p,
                  s_array_f8_24 burst_input,
                  s_array_f8_24 output_pp
                  ) {

    burst_input = make_input_array_f(burst_in);
    burst_p     = make_input_array_f(t_burst_p);

    int32 i4 = count(1000, 1);
    int32 i3 = count_chain(n3, 1, i4);
    int32 i2 = count_chain(n2, 1, i3);
    int32 i1 = count_chain(n1, Par, i2);

    float8_24 c_0   = -0.000000056202665632554272;
    float8_24 c_1_0 =  0.000000010666666661052204;
    float8_24 c_1_1 = -0.000000001523809300962853;
    float8_24 c_1_2 =  0.000000000253968179819708;
    float8_24 c_1_3 = -0.000000000031745984313547;
    float8_24 c_1_4 =  0.000000000002031744130576;
    float8_24 c_2_0 =  0.000000010666666661052204;
    float8_24 c_2_1 = -0.000000001523809300962853;
    float8_24 c_2_2 =  0.000000000253968179819708;
    float8_24 c_2_3 = -0.000000000031745984313547;
    float8_24 c_2_4 =  0.000000000002031744130576;
    float8_24 c_3_0 =  0.000000010666666661052204;
    float8_24 c_3_1 = -0.000000001523809300962853;
    float8_24 c_3_2 =  0.000000000253968179819708;
    float8_24 c_3_3 = -0.000000000031745984313547;
    float8_24 c_3_4 =  0.000000000002031744130576;
    float8_24 bc    = -0.0005;

    float8_24 up[Par];
    for (int i=0; i <Par; i++)
        up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;

    s_float8_24 p[Par];
    float8_24 pp_i[Par], dvv[Par], source[Par];

    s_float8_24 pb[Par];
    float8_24 ppb_i[Par], dvvb[Par];
    float8_24 image[1][1];

    for (int i=0; i < Par; i++) {
        cast2sff(p[i], burst_p[1*0+i], realType);
        cast2sff(pb[i], burst_p[1*1+i], realType);
    }

    for (int i=0; i <Par; i++) {
        cast2ff(pp_i[i], burst_input[Par * 0 + i], realType);
        cast2ff(dvv[i],  burst_input[Par * 1 + i], realType);
        cast2ff(source[i], burst_input[Par * 2 + i], realType);
        cast2ff(ppb_i[i], burst_input[Par * 3 + i], realType);
        cast2ff(dvvb[i], burst_input[Par * 4 + i], realType);
        cast2ff(image[i][0], burst_input[Par * 5 + i], realType);
    }

    //------------------------------boundary control--------------------------
    int32 f3 =(i3 <  SPONGE )? (SPONGE - i3) :
      ((i3 >= (n3 - SPONGE))? (i3-(n3-SPONGE)+1) : 0);
    int32 f3s= (f3 * f3);
    int32 f2 = i2 <  SPONGE        ? (SPONGE - i2) :
      ((i2 >= (n2 - SPONGE))? (i2-(n2-SPONGE)+1) : 0);
    int32 f2s= (f2 * f2);
    int32 f1 = i1*1 <  SPONGE        ? (SPONGE - i1*1) :
      ((i1 >= (n1 - SPONGE))? (i1*1-(n1-SPONGE)+1) : 0);
    int32 f1s= (f1 * f1);
    int32 is = f1s + f2s + f3s;

    pushRoundingMode();
    int32 it =  sqrt_i(0.0, 20.0, is, 32);
    popRoundingMode();

    int32 it_buf  = (it  < SPONGE) ? it : SPONGE;
    cast2ff(it_buf, it_buf, 8, 24);
    //    it_buf  =  it_buf.cast(hwFloat(8,24));

    int32 power = bc * it_buf;
    //    int32 pow   =(power).cast(hwFix(2, 30, HWFix.SignMode.TWOSCOMPLEMENT));
    int32 pow = power; //cast(power)

    //KernelMath.Range powrange = new KernelMath.Range(-0.1, 0.0);
    float8_24 scale = (it > 0) ? exp(pow) : 1;
    //    scale    = scale.cast(hwFloat(8,24));

    // TODO implement exponent

    //------------------------------boundary control--------------------------

    s_float8_24 inter[Par][Mul];

    float8_24 cur[Mul][11+Par+1][11][11], result[Par][Mul];

    s_offset nx  = make_offset(24 / Par, 48 / Par);
    s_offset nxy = make_offset(32 * nx, 32 * nx);


    //#pragma class:kernelopt name:pushDSP factor:1
    pushDSPFactor(DspFactor);

    //Cache
    for (int i=0; i < Par; i++) {
        int k = -6/Par;
        for (int x=-6; x<=6; x+=Par) {
            for (int y=-5; y<=5; y++)
                for (int z=-5; z<=5; z++)
                    cur[0][x+6+i][y+5][z+5] = p[i][z*nxy+y*nx+k];
            k++;
        }
    }

    //Computation
    for (int i=0; i <Par; i++) {
        //data-path(0,i)
        result[i][0] =(
                       cur[0][6+i][5][5] * 2.0 -
                       pp_i[i] + dvv[i]*(
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

        inter[i][0][0]  = fselect(up[i], pp_i[i],  result[i][0]);
    }
    //Multiple Time-Dimension
    for (int j=1; j <Mul; j++) {
        //Cache
        for (int i=0; i <Par; i++) {
            int k = -6/Par;
            for (int x=-6; x<=6; x+=Par) {
                for (int y=-5; y<=5; y++)
                    for (int z=-5; z<=5; z++)
                        cur[j][x+6+i][y+5][z+5] = inter[i][j-1][z*nxy+y*nx+k];
                k++;
            }
        }

        //Computation
        for (int i=0; i <Par; i++) {
            //data-path(j,i)
            result[i][j] = (
                            cur[j][6+i][5][5] * 2.0 -
                            cur[j-1][6+i][5][5] +
                            dvv[i]*(
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


            inter[i][j][0] = fselect(up[i], cur[j-1][6+i][5][5], result[i][j]);
        }
    }

#pragma class:kernelopt name:popDSP



    float8_24 curb[1][11 + Par + Mul][11][11], resultb[1][1];
    s_float8_24 interb[1][1];

    pushDSPFactor(1);
    //Cache
    for (int i=0; i <1; i++)
        {
            int k = -6/1;
            for (int x=-6; x<=6; x+=1)
                {
                    for (int y=-5; y<=5; y++)
                        for (int z=-5; z<=5; z++)
                            curb[0][x+6+i][y+5][z+5] = pb[i][z*nxy+y*nx+k];
                    k++;
                }
        }



    for (int i=0; i <1; i++) {
        ppb_i[i]     = ppb_i[i] * scale;
        resultb[i][0]=(
                       curb[0][6+i][5][5] * 2.0 -
                       ppb_i[i] +
                       dvvb[i]*(
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
        interb[i][0][0]  = (up[i]) ? resultb[i][0] : ppb_i[i];
    }

    for (int j=1; j <1; j++)
    {
      for (int i=0; i <1; i++)
      {
        int k = -6/1;
        for (int x=-6; x<=6; x+=1)
        {
          for (int y=-5; y<=5; y++)
            for (int z=-5; z<=5; z++)
             curb[j][x+6+i][y+5][z+5] = interb[i][j-1][z*nxy+y*nx+k];
          k++;
        }
      }

      for (int i=0; i <1; i++)
      {
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
      interb[i][j][0]  =(up[i])? resultb[i][j] : curb[j-1][6+i][5][5];
      }
    }
#pragma class:kernelopt name:popDSP

    for (int j =0; j< 1-1; j++)
	for (int i = 0; i < 1; i++)
	    image[i][j+1] = image[i][j] + inter[i][j][0] * interb[i][j][0];

    for (int i = 0; i < 1; i++)
	image[i][1-1]  = image[i][1-1] + inter[i][1-1][0] * interb[i][1-1][0];

    //------------------------------Data output --------------------------------

    // control counter
    s_array_f8_24 output_p        = make_array_f(t_burst_p);
    s_array_f8_24 burst_output    = make_array_f(burst_out);

    for (int i=0; i <1; i++) {
      output_p[Par*0+i][0] = castf_f(inter[i][1-1][0], 8, 24);
      output_p[Par*1+i][0] = castf_f(interb[i][1-1][0], 8,24);
    }

    for (int i=0; i <1; i++) {
      burst_output[Par*0+i][0] = castf_f(cur[1-1][6+i][5][5], 8,24);
      burst_output[Par*1+i][0] = castf_f(dvv[i], 8, 24);
      burst_output[Par*2+i][0] = castf_f(source[i], 8, 24);
      burst_output[Par*3+i][0] = castf_f(curb[1-1][6+i][5][5], 8,24);
      burst_output[Par*4+i][0] = castf_f(dvvb[i], 8,24);
      burst_output[Par*5+i][0] = castf_f(image[i][1-1], 8,24);
    }

    output_iaf(output_p, output_p, t_burst_p);
    output_iaf(output_pp, burst_output, burst_out);

}
