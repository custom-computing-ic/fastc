#include "../../../include/maxcc.h"
#include "../include/params_dse.h"

#pragma class:scalar dir:in name:n1 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:n2 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:n3 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:ORDER type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:SPONGE type:uint32 func:kernel_RTM
//#pragma class:array  dir:in name:burst_p type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:in name:burst_pp type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:in name:burst_dvv type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:in name:burst_source type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:out name:output_p type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:out name:ker_p type:float8_24 width:1 func:kernel_RTM
//#pragma class:array  dir:out name:output_pp type:float8_24 width:1 func:kernel_RTM
void kernel_RTM(
                s_uint32 n1, s_uint32 n2, s_uint32 n3,
		s_uint32 ORDER, s_uint32 SPONGE,
		s_array_f8_24 burst_p,
		s_array_f8_24 burst_dvv,
		s_array_f8_24 burst_pp,
		s_array_f8_24 burst_source,
		s_array_f8_24 output_pp,
		s_array_f8_24 ker_p
                ) {

    burst_p = make_input_array_f(8, 24, Par);
    burst_pp  = make_input_array_f(8, 24, Par);
    burst_dvv = make_input_array_f(8, 24, Par);
    burst_source = make_input_array_f(8, 24, Par);

    int32 i4 = count(1000, 1);
    int32 i3 = count_chain(n3, 1, i4);
    int32 i2 = count_chain(n2, 1, i3);
    int32 i1 = count_chain(n1, Par, i2);

    float8_24 c_0 = -0.000000056202665632554272;
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

    float8_24 up[Par];
    for (int i=0; i <Par; i++)
        up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;

    s_float8_24 p[Par];
    float8_24 pp_i[Par], dvv[Par], source[Par];

    for (int i=0; i <Par; i++) {
      cast2sff(p[i], burst_p[i], realType);
      cast2fsf(pp_i[i], burst_pp[i], realType);
      cast2fsf(dvv[i],  burst_dvv[i], realType);
      cast2fsf(source[i], burst_source[i], realType);
    }

    s_float8_24 inter[Par][Mul];

    float8_24 cur[Mul][11+Par+1][11][11], result[Par][Mul];

    s_offset nx  = make_offset(min_nx / Par, max_nx / Par);
    s_offset nxy = make_offset(dim_y * nx, dim_y * nx);

    // setup the optimisation factor to transfer operations between DSPs and Luts

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
                      cur[0][6+i][5][5] * 2.0 - pp_i[i] + dvv[i]*(
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
         result[i][j] =(
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


         inter[i][j][0] = fselect(up[i], cur[j-1][6+i][5][5], result[i][j]);
       }
     }

#pragma class:kernelopt name:popDSP

  //------------------------------Data output --------------------------------

  // control counter
    s_array_f8_24 output_pp_inter = make_array_f(8, 24, Par);
    s_array_f8_24 output_p        = make_array_f(8, 24, Par);

  for (int i=0; i <Par; i++) {
    output_p[i][0]  = castf_f(inter[i][Mul-1][0], 8, 24);
    output_pp_inter[i][0] = castf_f(cur[Mul-1][6+i][5][5], 8 ,24);
  }

  output_iaf(ker_p, output_p, 8, 24 , Par);
  output_iaf(output_pp, output_pp_inter, 8, 24, Par);

}
