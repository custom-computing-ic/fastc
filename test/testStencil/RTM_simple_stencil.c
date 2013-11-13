void kernel_f1(int *a, int *b) {
  b[0] = a[1] + 1;
}

void kernel_f2(int *a, int *b) {
  b[0] = a[1] * 5;
}

void kernel_f3(int * a, int *b, int *c) {
  c[0] = a[1]  + b[2];
}

#define N1 128
#define N12 16384
void kernel_do_step(float* dvv, float * p, float *pp, float *pp_out, float *conv,
                    float *source,
                    int n1, int n2, int n3, int n12, int ORDER,
                    float c_0, float c_1_0, float c_1_1, float c_1_2, float c_1_3,
                    float c_1_4, float c_2_0, float c_2_1, float c_2_2, float c_2_3,
                    float c_2_4, float c_3_0, float c_3_1, float c_3_2,
                    float c_3_3, float c_3_4) {

#pragma fast stencil
  for(int i3=ORDER; i3 < n3-ORDER; i3++){
    for(int i2=ORDER; i2 < n2-ORDER; i2++){
      for(int i1=ORDER; i1< n1-ORDER; i1++){

        conv[i1+i2*N1+i3*N12] =
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

        pp_out[i1+i2*N1+i3*N12] =
          2.0 * p[i1+i2*N1+i3*N12]-pp[i1+i2*N1+i3*N12] +
          conv[i1+i2*N1+i3*N12] + source[i1+i2*N1+i3*N12];

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


  int size = 384;
  int *in = (int *)malloc(sizeof(int) * size);

  for (int i = 0; i < 10; i++)
    in[i] = 0;

  int *out1 = (int *)malloc(sizeof(int) * size);
  int *out2 = (int *)malloc(sizeof(int) * size);
  int *out3 = (int *)malloc(sizeof(int) * size);
  int *out4 = (int *)malloc(sizeof(int) * size);

  //the output must be declared before input!
#pragma fast hw:kernel_f1
  kernel_f1(in, out1);

#pragma fast hw:kernel_f3
  kernel_f3(out1, out2, out3);

#pragma fast hw:kernel_f3
  kernel_f3(out1, out3, out4);

  float *dvv = (float *)malloc(sizeof(float) * n123);

#pragma fast hw:kernel_f2
  kernel_f2(out3, dvv);

  for (int i = 0; i < 10; i++)
    if (out3[i] != 6) {
      //      cout << "Error" << endl;
      return 1;
    }



  float *p = (float *)malloc(sizeof(float) * n123);
  float *pp = (float *)malloc(sizeof(float) * n123);
  float *pp_out = (float *)malloc(sizeof(float) * n123);
  float *source_container = (float *)malloc(sizeof(float) * n123);
  float *conv =(float*)malloc(sizeof(float)*n123);
  float *cal =(float*)malloc(sizeof(float)*n123);

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

#pragma fast hw_kernel:kernel_do_step
  kernel_do_step(dvv, p, pp, pp_out, conv, source_container,
                 n1, n2, n3, n12, ORDER,
                 c_0,  c_1_0,  c_1_1,  c_1_2,  c_1_3,
                 c_1_4,  c_2_0,  c_2_1,  c_2_2,  c_2_3,
                 c_2_4,  c_3_0,  c_3_1,  c_3_2,
                 c_3_3,  c_3_4);
  return 0;

}
