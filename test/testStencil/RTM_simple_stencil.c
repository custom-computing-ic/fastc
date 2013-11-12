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
  float *source_container = (float *)malloc(sizeof(float) * n123);

  float c_0 = 0;
  float c_1[] = {0, 0, 0, 0, 0};
  float c_2[] = {0, 0, 0, 0, 0};
  float c_3[] = {0, 0, 0, 0, 0};	


  float *conv =(float*)malloc(sizeof(float)*n123);
  float *cal =(float*)malloc(sizeof(float)*n123);

#pragma fast stencil
  for(int i3=ORDER; i3 < n3-ORDER; i3++){  
    for(int i2=ORDER; i2 < n2-ORDER; i2++){
      for(int i1=ORDER; i1< n1-ORDER; i1++){

	conv[i1+i2*n1+i3*n12] =
	  dvv[i1+i2*n1+i3*n12]
	  *(
	    p[i1+i2*n1+i3*n12]*c_0
	    +c_1[0]*(p[(i1+1)+(i2  )*n1+(i3  )*n12]+p[(i1-1)+(i2  )*n1+(i3  )*n12]));
	/*
	    +c_1[1]*(p[(i1+2)+(i2  )*n1+(i3  )*n12]+p[(i1-2)+(i2  )*n1+(i3  )*n12])
	    +c_1[2]*(p[(i1+3)+(i2  )*n1+(i3  )*n12]+p[(i1-3)+(i2  )*n1+(i3  )*n12])
	    +c_1[3]*(p[(i1+4)+(i2  )*n1+(i3  )*n12]+p[(i1-4)+(i2  )*n1+(i3  )*n12])
	    +c_1[4]*(p[(i1+5)+(i2  )*n1+(i3  )*n12]+p[(i1-5)+(i2  )*n1+(i3  )*n12])
	    +c_2[0]*(p[(i1  )+(i2+1)*n1+(i3  )*n12]+p[(i1  )+(i2-1)*n1+(i3  )*n12])
	    +c_2[1]*(p[(i1  )+(i2+2)*n1+(i3  )*n12]+p[(i1  )+(i2-2)*n1+(i3  )*n12])
	    +c_2[2]*(p[(i1  )+(i2+3)*n1+(i3  )*n12]+p[(i1  )+(i2-3)*n1+(i3  )*n12])
	    +c_2[3]*(p[(i1  )+(i2+4)*n1+(i3  )*n12]+p[(i1  )+(i2-4)*n1+(i3  )*n12])
	    +c_2[4]*(p[(i1  )+(i2+5)*n1+(i3  )*n12]+p[(i1  )+(i2-5)*n1+(i3  )*n12])
	    +c_3[0]*(p[(i1  )+(i2  )*n1+(i3+1)*n12]+p[(i1  )+(i2  )*n1+(i3-1)*n12])
	    +c_3[1]*(p[(i1  )+(i2  )*n1+(i3+2)*n12]+p[(i1  )+(i2  )*n1+(i3-2)*n12])
	    +c_3[2]*(p[(i1  )+(i2  )*n1+(i3+3)*n12]+p[(i1  )+(i2  )*n1+(i3-3)*n12])
	    +c_3[3]*(p[(i1  )+(i2  )*n1+(i3+4)*n12]+p[(i1  )+(i2  )*n1+(i3-4)*n12])
	    +c_3[4]*(p[(i1  )+(i2  )*n1+(i3+5)*n12]+p[(i1  )+(i2  )*n1+(i3-5)*n12]));
	    */
        pp[i1+i2*n1+i3*n12] = 
	  2.0 * p[i1+i2*n1+i3*n12]-pp[i1+i2*n1+i3*n12] + 
	  conv[i1+i2*n1+i3*n12] + source_container[i1+i2*n1+i3*n12];  
	
      }
    }
  }

  free(conv);
  free(cal);

  return 0;
}
