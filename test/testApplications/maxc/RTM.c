#define N1 128
#define N12 16384

// stencil operator constants
const float c_0   = -0.000000056202665632554272;
const float c_1_0 =  0.000000010666666661052204;
const float c_1_1 = -0.000000001523809300962853;
const float c_1_2 =  0.000000000253968179819708;
const float c_1_3 = -0.000000000031745984313547;
const float c_1_4 =  0.000000000002031744130576;
const float c_2_0 =  0.000000010666666661052204;
const float c_2_1 = -0.000000001523809300962853;
const float c_2_2 =  0.000000000253968179819708;
const float c_2_3 = -0.000000000031745984313547;
const float c_2_4 =  0.000000000002031744130576;
const float c_3_0 =  0.000000010666666661052204;
const float c_3_1 = -0.000000001523809300962853;
const float c_3_2 =  0.000000000253968179819708;
const float c_3_3 = -0.000000000031745984313547;
const float c_3_4 =  0.000000000002031744130576;
const float scale =  0.000000000002031744130576;


void kernel_do_forward(float* dvv, float * p, float *pp, float *pp_out, float *p_out,
                       float *source, int n1, int n2, int n3, int n12, int ORDER) {

#pragma fast stencil
  for(int i3=ORDER; i3 < n3-ORDER; i3++){
    for(int i2=ORDER; i2 < n2-ORDER; i2++){
      for(int i1=ORDER; i1< n1-ORDER; i1++){
        pp_out[i1+i2*N1+i3*N12] =
          2.0 * p[i1+i2*N1+i3*N12]-
          pp[i1+i2*N1+i3*N12] +
          source[i1+i2*N1+i3*N12] +
          dvv[i1+i2*N1+i3*N12]
          *(
            p[i1+i2*N1+i3*N12]*c_0
            +c_1_0*(p[(i1+1)+(i2  )*N1+(i3  )*N12]+p[(i1-1)+(i2  )*N1+(i3  )*N12])
            +c_1_1*(p[(i1+2)+(i2  )*N1+(i3  )*N12]+p[(i1-2)+(i2  )*N1+(i3  )*N12])
            +c_1_2*(p[(i1+3)+(i2  )*N1+(i3  )*N12]+p[(i1-3)+(i2  )*N1+(i3  )*N12])
            +c_1_3*(p[(i1+4)+(i2  )*N1+(i3  )*N12]+p[(i1-4)+(i2  )*N1+(i3  )*N12])
            +c_1_4*(p[(i1+5)+(i2  )*N1+(i3  )*N12]+p[(i1-5)+(i2  )*N1+(i3  )*N12])
            +c_2_0*(p[(i1  )+(i2+1)*N1+(i3  )*N12]+p[(i1  )+(i2-1)*N1+(i3  )*N12])
            +c_2_1*(p[(i1  )+(i2+2)*N1+(i3  )*N12]+p[(i1  )+(i2-2)*N1+(i3  )*N12])
            +c_2_2*(p[(i1  )+(i2+3)*N1+(i3  )*N12]+p[(i1  )+(i2-3)*N1+(i3  )*N12])
            +c_2_3*(p[(i1  )+(i2+4)*N1+(i3  )*N12]+p[(i1  )+(i2-4)*N1+(i3  )*N12])
            +c_2_4*(p[(i1  )+(i2+5)*N1+(i3  )*N12]+p[(i1  )+(i2-5)*N1+(i3  )*N12])
            +c_3_0*(p[(i1  )+(i2  )*N1+(i3+1)*N12]+p[(i1  )+(i2  )*N1+(i3-1)*N12])
            +c_3_1*(p[(i1  )+(i2  )*N1+(i3+2)*N12]+p[(i1  )+(i2  )*N1+(i3-2)*N12])
            +c_3_2*(p[(i1  )+(i2  )*N1+(i3+3)*N12]+p[(i1  )+(i2  )*N1+(i3-3)*N12])
            +c_3_3*(p[(i1  )+(i2  )*N1+(i3+4)*N12]+p[(i1  )+(i2  )*N1+(i3-4)*N12])
            +c_3_4*(p[(i1  )+(i2  )*N1+(i3+5)*N12]+p[(i1  )+(i2  )*N1+(i3-5)*N12]));
        p_out[i1+i2*N1+i3*N12] = pp[i1+i2*N1+i3*N12];
      }
    }
  }
}

void kernel_do_backward(float* dvv, float * p, float *pp, float *pp_out, float *p_out,
                        float *source, float scale,
                        int n1, int n2, int n3, int n12, int ORDER) {

#pragma fast stencil
  for(int i3=ORDER; i3 < n3-ORDER; i3++){
    for(int i2=ORDER; i2 < n2-ORDER; i2++){
      for(int i1=ORDER; i1< n1-ORDER; i1++){

        pp_out[i1 + i2 * N1 + i1 * N12] =
          2.0 * p[i1+i2*N1+i3*N12]-
          pp[i1+i2*N1+i3*N12] * scale+
          source[i1+i2*N1+i3*N12] +
          dvv[i1+i2*N1+i3*N12]
          *(
            p[i1+i2*N1+i3*N12]*c_0
            +c_1_0*(p[(i1+1)+(i2  )*N1+(i3  )*N12]+p[(i1-1)+(i2  )*N1+(i3  )*N12])
            +c_1_1*(p[(i1+2)+(i2  )*N1+(i3  )*N12]+p[(i1-2)+(i2  )*N1+(i3  )*N12])
            +c_1_2*(p[(i1+3)+(i2  )*N1+(i3  )*N12]+p[(i1-3)+(i2  )*N1+(i3  )*N12])
            +c_1_3*(p[(i1+4)+(i2  )*N1+(i3  )*N12]+p[(i1-4)+(i2  )*N1+(i3  )*N12])
            +c_1_4*(p[(i1+5)+(i2  )*N1+(i3  )*N12]+p[(i1-5)+(i2  )*N1+(i3  )*N12])
            +c_2_0*(p[(i1  )+(i2+1)*N1+(i3  )*N12]+p[(i1  )+(i2-1)*N1+(i3  )*N12])
            +c_2_1*(p[(i1  )+(i2+2)*N1+(i3  )*N12]+p[(i1  )+(i2-2)*N1+(i3  )*N12])
            +c_2_2*(p[(i1  )+(i2+3)*N1+(i3  )*N12]+p[(i1  )+(i2-3)*N1+(i3  )*N12])
            +c_2_3*(p[(i1  )+(i2+4)*N1+(i3  )*N12]+p[(i1  )+(i2-4)*N1+(i3  )*N12])
            +c_2_4*(p[(i1  )+(i2+5)*N1+(i3  )*N12]+p[(i1  )+(i2-5)*N1+(i3  )*N12])
            +c_3_0*(p[(i1  )+(i2  )*N1+(i3+1)*N12]+p[(i1  )+(i2  )*N1+(i3-1)*N12])
            +c_3_1*(p[(i1  )+(i2  )*N1+(i3+2)*N12]+p[(i1  )+(i2  )*N1+(i3-2)*N12])
            +c_3_2*(p[(i1  )+(i2  )*N1+(i3+3)*N12]+p[(i1  )+(i2  )*N1+(i3-3)*N12])
            +c_3_3*(p[(i1  )+(i2  )*N1+(i3+4)*N12]+p[(i1  )+(i2  )*N1+(i3-4)*N12])
            +c_3_4*(p[(i1  )+(i2  )*N1+(i3+5)*N12]+p[(i1  )+(i2  )*N1+(i3-5)*N12]));
        p_out[i1+i2*N1+i3*N12] = pp[i1+i2*N1+i3*N12];
      }
    }
  }
}

int main() {

  int n1 = 128;
  int n2 = 128;
  int n3 = 128;
  int ORDER = 11;
  int n12 = n1 * n2;
  int n123 = n1 * n2 * n3;

  float *dvv = (float *)malloc(sizeof(float) * n123);
  float *p = (float *)malloc(sizeof(float) * n123);
  float *pp = (float *)malloc(sizeof(float) * n123);
  float *pp_out = (float *)malloc(sizeof(float) * n123);
  float *pp_out1 = (float *)malloc(sizeof(float) * n123);
  float *pp_out2 = (float *)malloc(sizeof(float) * n123);
  float *pp_out3 = (float *)malloc(sizeof(float) * n123);
  float *pp_out4 = (float *)malloc(sizeof(float) * n123);
  float *source_container = (float *)malloc(sizeof(float) * n123);
  float *conv =(float*)malloc(sizeof(float)*n123);
  float *cal =(float*)malloc(sizeof(float)*n123);



#pragma fast hw_kernel:kernel_do_forward
  kernel_do_forward(dvv, p, pp, pp_out, conv, source_container,
                    n1, n2, n3, n12, ORDER);

#pragma fast hw_kernel:kernel_do_forward
<<<<<<< HEAD
  kernel_do_forward(dvv, conv, pp_out, pp_out1, pp_out2, source_container,
                    n1, n2, n3, n12, ORDER,
                    c_0,  c_1_0,  c_1_1,  c_1_2,  c_1_3,
                    c_1_4,  c_2_0,  c_2_1,  c_2_2,  c_2_3,
                    c_2_4,  c_3_0,  c_3_1,  c_3_2,
                    c_3_3,  c_3_4);

#pragma fast hw_kernel:kernel_do_backward
  kernel_do_backward(dvv, conv, pp_out,pp_out3, pp_out4, source_container, scale,
                     n1, n2, n3, n12, ORDER,
                     c_0,  c_1_0,  c_1_1,  c_1_2,  c_1_3,
                     c_1_4,  c_2_0,  c_2_1,  c_2_2,  c_2_3,
                     c_2_4,  c_3_0,  c_3_1,  c_3_2,
                     c_3_3,  c_3_4);
=======
  kernel_do_forward(dvv, pp_out, pp_out1, pp_out1, conv, source_container,
                    n1, n2, n3, n12, ORDER);

#pragma fast hw_kernel:kernel_do_backward
  kernel_do_backward(dvv, pp_out, pp_out2,pp_out2, conv, source_container, scale,
                     n1, n2, n3, n12, ORDER);
>>>>>>> 450fb10ccd5b1b4f89164b2bdca3eaa466262697
  return 0;
}
