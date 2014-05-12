typedef struct RT_Point_s {
  float c[3];
} RT_Point;

typedef struct
{
  unsigned int    nVertices;
  char            projectionDimension[2];
  RT_Point        *vertexTab;
//RT_Normal     normal;
//RT_Vector     u,v;
//RT_FP       uMinMax[2], vMinMax[2];
//RT_FP       elevation;
//RT_Transformation xyz2uvn;
} RT_Polygon;



void   kernel_IsPointInsidePolygon(const RT_Polygon *polygon, const RT_Point *p, RT_Point *e0, RT_Point *e1, int *inside_out)
{
  int       i0 = 1;
  int       i1 = 2;
  unsigned int  n;
  int       y0, y1;
  int       inside1 = 0;
  int inside2 = 0;

  for(n=1; n<=polygon->nVertices; n++)
  {
    y1 = (e1->c[i1] >= p->c[i1]) ? 1 : 0;
    inside2 = ((y0 != y1) &&(((e1->c[i1]-p->c[i1]) * (e0->c[i0]-e1->c[i0]) >= (e1->c[i0]-p->c[i0]) * (e0->c[i1]-e1->c[i1]) ) == y1)) ?
       ((inside1 == 1) ? 0 : 1) : inside1;
    inside1 = inside2;
    y0 = y1;
    
    e0->c[0] = e1->c[0];
    e0->c[1] = e1->c[1];
    e0->c[2] = e1->c[2];

    int poly = (n != polygon->nVertices);
    e1->c[0] = poly? (polygon->vertexTab[n]).c[0]:e1->c[0];
    e1->c[1] = poly? (polygon->vertexTab[n]).c[1]:e1->c[1];
    e1->c[2] = poly? (polygon->vertexTab[n]).c[2]:e1->c[2];


  }

  inside_out[0] = inside2;
}

int main(){

  const RT_Polygon *polygon;
  const RT_Point *p;
  int* inside2;

  #pragma fast hw_kernel:kernel_IsPointInsidePolygon
  kernel_IsPointInsidePolygon(polygon, p, p, p , inside2);

}
