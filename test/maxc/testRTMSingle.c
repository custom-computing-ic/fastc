typedef float float8_24;
typedef float* s_float8_24;
typedef int* s_uint32_in;
typedef int** s_array_f8_24;
typedef int s_uint32;
typedef int int32;
typedef int uint32;
typedef int s_offset;

#define Par 1
#define Mul 1

#pragma class:scalar dir:in name:n1 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:n2 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:n3 type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:ORDER type:uint32 func:kernel_RTM
#pragma class:scalar dir:in name:SPONGE type:uint32 func:kernel_RTM
#pragma class:array  dir:in name:burst_p type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:in name:burst_pp type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:in name:burst_dvv type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:in name:burst_source type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:out name:output_p type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:out name:ker_p type:float8_24 width:1 func:kernel_RTM
#pragma class:array  dir:out name:output_pp type:float8_24 width:1 func:kernel_RTM
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

    int32 i4 = count(1000, 1);
    int32 i3 = countChain(n3, 1, i4);
    int32 i2 = countChain(n2, 1, i3);
    int32 i1 = countChain(n1, Par, i2);

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
    float8_24 bc = -0.0005;

    float8_24 up[Par];
    for (int i=0; i <Par; i++)
        up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;

    uint32 output_ring = n3  > i3  & i3 >= n3 - ORDER;
    uint32 input_ring = ORDER > i3;

    uint32 output_enable = (n3 - ORDER > i3 & i3 >= ORDER);

    s_float8_24 p[Par], pp_i[Par], dvv[Par], source[Par];
    float8_24 image[Par][Mul];

    for (int i=0; i <Par; i++) {
        p[i][0]      =  burst_p[i][0];
        pp_i[i][0]   =  burst_pp[i][0];
        dvv[i][0]    =  burst_dvv[i][0];
        source[i][0] =  burst_source[i][0];
    }

    s_float8_24 inter[Par][Mul];

    float8_24 cur[Mul][11+Par+1][11][11], result[Par][Mul];

    s_offset nx  = make_offset("nx", 24, 48);
    s_offset nxy = make_offset("nxy", 32 * nx, 32 * nx);

    // setup the optimisation factor to transfer operations between DSPs and Luts
//#pragma class:opt name:pushDSP factor:1
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
    /*
    //Computation
    for (int i=0; i <Par; i++) {
        //data-path(0,i)
        result[i][0][0] =(
                      cur[0][6+i][5][5][0] * 2.0 - pp_i[i][0] + dvv[i][0]*(
              cur[0][6+i][5][5][0] * c_0
              +(cur[0][5+i][5][5][0] + cur[0][7+i][5][5][0]) * c_1_0
              +(cur[0][4+i][5][5][0] + cur[0][8+i][5][5][0]) * c_1_1
              +(cur[0][3+i][5][5][0] + cur[0][9+i][5][5][0]) * c_1_2
              +(cur[0][2+i][5][5][0] + cur[0][10+i][5][5][0])* c_1_3
              +(cur[0][1+i][5][5][0] + cur[0][11+i][5][5][0])* c_1_4
              +(cur[0][6+i][4][5][0] + cur[0][6+i][6][5][0]) * c_2_0
              +(cur[0][6+i][3][5][0] + cur[0][6+i][7][5][0]) * c_2_1
              +(cur[0][6+i][2][5][0] + cur[0][6+i][8][5][0]) * c_2_2
              +(cur[0][6+i][1][5][0] + cur[0][6+i][9][5][0]) * c_2_3
              +(cur[0][6+i][0][5][0] + cur[0][6+i][10][5][0])* c_2_4
              +(cur[0][6+i][5][4][0] + cur[0][6+i][5][6][0]) * c_3_0
              +(cur[0][6+i][5][3][0] + cur[0][6+i][5][7][0]) * c_3_1
              +(cur[0][6+i][5][2][0] + cur[0][6+i][5][8][0]) * c_3_2
              +(cur[0][6+i][5][1][0] + cur[0][6+i][5][9][0]) * c_3_3
              +(cur[0][6+i][5][0][0] + cur[0][6+i][5][10][0])* c_3_4 ))
             + source[i][0];

	//     inter[i][0][0]  = fselect(up[i], pp_i[i][0],  result[i][0]);
	} */
    //Multiple Time-Dimension
    /*for (int j=1; j <Mul; j++) {
       //Cache
       for (int i=0; i <Par; i++) {
         int k = -6/Par;
         for (int x=-6; x<=6; x+=Par) {
           for (int y=-5; y<=5; y++)
             for (int z=-5; z<=5; z++)
               cur[j][x+6+i][y+5][z+5][0] = inter[i][j-1][z*nxy+y*nx+k];
           k++;
         }
       }

       //Computation
       for (int i=0; i <Par; i++) {
         //data-path(j,i)
         result[i][j][0] =(
                cur[j][6+i][5][5][0] * 2.0 - cur[j-1][6+i][5][5][0] +dvv[i][0]*(
                cur[j][6+i][5][5][0] * c_0
              +(cur[j][5+i][5][5][0] + cur[j][7+i][5][5][0]) * c_1_0
              +(cur[j][4+i][5][5][0] + cur[j][8+i][5][5][0]) * c_1_1
              +(cur[j][3+i][5][5][0] + cur[j][9+i][5][5][0]) * c_1_2
              +(cur[j][2+i][5][5][0] + cur[j][10+i][5][5][0]) * c_1_3
              +(cur[j][1+i][5][5][0] + cur[j][11+i][5][5][0])* c_1_4
              +(cur[j][6+i][4][5][0] + cur[j][6+i][6][5][0]) * c_2_0
              +(cur[j][6+i][3][5][0] + cur[j][6+i][7][5][0]) * c_2_1
              +(cur[j][6+i][2][5][0] + cur[j][6+i][8][5][0]) * c_2_2
              +(cur[j][6+i][1][5][0] + cur[j][6+i][9][5][0]) * c_2_3
              +(cur[j][6+i][0][5][0] + cur[j][6+i][10][5][0])* c_2_4
              +(cur[j][6+i][5][4][0] + cur[j][6+i][5][6][0]) * c_3_0
              +(cur[j][6+i][5][3][0] + cur[j][6+i][5][7][0]) * c_3_1
              +(cur[j][6+i][5][2][0] + cur[j][6+i][5][8][0]) * c_3_2
              +(cur[j][6+i][5][1][0] + cur[j][6+i][5][9][0]) * c_3_3
              +(cur[j][6+i][5][0][0] + cur[j][6+i][5][10][0])* c_3_4 ))
               + source[i][0];
         inter[i][j][0] = up[i] ? result[i][j][0] : cur[j-1][6+i][5][5][0];
       }
     }

//#pragma class:opt name:popDSP

  //------------------------------Data output --------------------------------

  // control counter
  s_array_f8_24 output_p_inter, output_pp_inter, output_p;

  for (int i=0; i <Par; i++) {
     output_p[i][0]  = inter[i][Mul-1][0];
     output_pp_inter[i][0] = cur[Mul-1][6+i][5][5][0];
  }

  output_iaf(ker_p, output_p);
  output_iaf(output_pp, output_pp_inter);
*/
}
