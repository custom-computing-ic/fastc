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



int   kernel_IsPointInsidePolygon(const RT_Polygon *polygon, const RT_Point *p)
{
  int       i0 = polygon->projectionDimension[0];
  int       i1 = polygon->projectionDimension[1];
  unsigned int  n;
  RT_Point    e0, e1;
  int       y0, y1;
  int       inside = 0;


  for(n=1; n<=polygon->nVertices; n++)
  {
    y1 = (e1.c[i1] >= p->c[i1]) ? 1 : 0;
    if (y0 != y1)
      if (( (e1.c[i1]-p->c[i1]) * (e0.c[i0]-e1.c[i0]) >= (e1.c[i0]-p->c[i0]) * (e0.c[i1]-e1.c[i1]) ) == y1)
        inside = (inside == 1) ? 0 : 1;
    y0 = y1;

    e0.c[0] = e1.c[0];
    e0.c[1] = e1.c[1];
    e0.c[2] = e1.c[2];

    if (n != polygon->nVertices)
    {
      e1.c[0] = (polygon->vertexTab[n]).c[0];
      e1.c[1] = (polygon->vertexTab[n]).c[1];
      e1.c[2] = (polygon->vertexTab[n]).c[2];
    }
  }

  return inside;
}

int main(){

  const RT_Polygon *polygon;
  const RT_Point *p;

  #pragma fast hw_kernel:kernel_IsPointInsidePolygon
  kernel_IsPointInsidePolygon(polygon, p);

}
