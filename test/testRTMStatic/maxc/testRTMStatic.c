#include "../../../include/maxcc.h"
//#include "../include/params_dse.h"

// design parameters
const int Par = 1;
const int Mul = 1;
const int DspFactor = 1;
const int min_nx = 24;
const int max_nx = 48;
const int dim_y = 32;

#define realType 8, 24


#pragma fast var:nx type:offset max:max_nx/Par min:min_nx/Par func:kernel_RTM
#pragma fast var:nxy type:offset max:dim_y*nx min:dim_y*nx func:kernel_RTM
#pragma fast var:burst_p type:float(8, 24) func:kernel_RTM
void kernel_RTM(
                unsigned int n1, unsigned int n2, unsigned int n3,
		unsigned int ORDER, unsigned int SPONGE,
		float* burst_p[Par],
		float* burst_dvv[Par],
		float* burst_pp[Par],
		float* burst_source[Par],
		float* output_pp[Par],
		float* ker_p[Par],
                int nx, int nxy
                )
{

    int i4 = count(1000, 1);
    int i3 = count_chain(n3, 1, i4);
    int i2 = count_chain(n2, 1, i3);
    int i1 = count_chain(n1, Par, i2);

    float c_0   = -0.000000056202665632554272;
    float c_1_0 =  0.000000010666666661052204;
    float c_1_1 = -0.000000001523809300962853;
    float c_1_2 =  0.000000000253968179819708;
    float c_1_3 = -0.000000000031745984313547;
    float c_1_4 =  0.000000000002031744130576;
    float c_2_0 =  0.000000010666666661052204;
    float c_2_1 = -0.000000001523809300962853;
    float c_2_2 =  0.000000000253968179819708;
    float c_2_3 = -0.000000000031745984313547;
    float c_2_4 =  0.000000000002031744130576;
    float c_3_0 =  0.000000010666666661052204;
    float c_3_1 = -0.000000001523809300962853;
    float c_3_2 =  0.000000000253968179819708;
    float c_3_3 = -0.000000000031745984313547;
    float c_3_4 =  0.000000000002031744130576;

    float up[Par];
    for (int i=0; i <Par; i++)
        up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;

    float* p[Par], *inter[Par][Mul];
    float pp_i[Par], dvv[Par], source[Par], cur[Mul][11+Par+1][11][11], result[Par][Mul];

    for (int i=0; i <Par; i++) {
      cast2sff(p[i], burst_p[i], realType);
      cast2fsf(pp_i[i], burst_pp[i], realType);
      cast2fsf(dvv[i],  burst_dvv[i], realType);
      cast2fsf(source[i], burst_source[i], realType);
    }

#pragma class:kernelopt name:pushDSP factor:DspFactor

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

	inter[i][0][0]  = up[i] == 0 ? pp_i[i] : result[i][0];
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

         inter[i][j][0] = up[i] == 0 ? cur[j-1][6+i][5][5] : result[i][j];
       }
     }

#pragma class:kernelopt name:popDSP

  //------------------------------Data output --------------------------------

    for (int i=0; i <Par; i++) {
      ker_p[i][0] = inter[i][Mul-1][0];
      output_pp[i][0] = cur[Mul-1][6+i][5][5];
    }

}
