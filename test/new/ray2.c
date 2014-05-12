/*
int IntersectSingleVoxel(RT_Voxel *voxel, RT_Ray *ray, RT_Intersection *intersection) {
  int       hit = RT_FALSE, n;
  RT_Intersection localIntersection;

  localIntersection.distance = RT_FP_MAX;
  for(n=0; n<voxel->nPrimitives; n++) {
    if (IntersectPrimitive(voxel->primitiveArray[n], ray, &localIntersection)) {
      hit = RT_TRUE;
      if (localIntersection.distance < intersection->distance)
        InitIntersectionWithIntersection(intersection, &localIntersection);
    }
  }
  return hit;
}
*/
#define RT_FP     float

typedef struct RT_Point_s {
    float c[3];
} RT_Point;

typedef struct RT_ListC
{
  void        *body;
  struct RT_ListC   *prev;
  struct RT_ListC   *next;
} RT_ListCell;

typedef struct
{
  unsigned int  elementCount;
  RT_ListCell   *head, *tail, *current;
} RT_List;


typedef struct RT_Voxel_s {
  RT_List     primitiveList;
  //RT_Primitive  **primitiveArray;
  unsigned int  nPrimitives;
} RT_Voxel;

typedef struct
{
  RT_Point  min, max, center;
  RT_FP   size[3];
} RT_BBox;


typedef struct RT_Grid_s {
  RT_BBox     bbox;
  unsigned int  size[3];
  RT_FP     delta[3];
  RT_FP     deltaInv[3];
  RT_Voxel    **voxels;
  unsigned int  nVoxels;
} RT_Grid;

typedef struct RT_Vector_s {
    RT_FP c[3];
} RT_Vector;

enum RT_RayType {
  RT_primaryRay,
  RT_shadowRay,
  RT_diffuseRay,
  RT_specularRay,
};

typedef struct
{
  unsigned int  ID;
  RT_Point    source;
  RT_Vector   direction;
  RT_Vector   directionInv;
  unsigned char depth;
  enum RT_RayType type;
  RT_FP     d_in, d_out;
} RT_Ray;


//int kernel_Intersect(RT_Grid *grid, RT_Ray *ray, RT_Intersection *intersection) {
int kernel_Intersect(RT_Grid *grid, RT_Ray *ray,const int *cmpToAxis, int *hit_out) {
  float             rayT;
  RT_Point          p, gridIntersect;
  //RT_Intersection   localIntersection;
  float       nextCrossing[3], deltaT[3];
  int         step[3], out[3], pos[3], axis, stepAxis, bits;
  int         hit = 0;
  RT_Voxel      *voxel;
  int         stop1=0;
  int         stop2=0;
  int         stop;

  //------------------------------------------------------------------------------
  // Traverse the voxels along the ray
  //------------------------------------------------------------------------------
  for(;;) {
    //------------------------------------------------------------------------------
    // Fetch the current voxel
    //------------------------------------------------------------------------------
    voxel = grid->voxels[(pos[2] * grid->size[1]+ pos[1])* grid->size[0]+pos[0]];
    bits = ((nextCrossing[0] < nextCrossing[1])<<2) + ((nextCrossing[0] < nextCrossing[2])<<1) + ((nextCrossing[1] <  nextCrossing[2]));
    stepAxis = cmpToAxis[bits];

    //-- ray section out of the voxel 
    stop1 = ray->d_out < nextCrossing[stepAxis];

    //-- go to next voxel
    pos[stepAxis] += step[stepAxis];

    //-- exit if out of voxel grid
    stop2 =pos[stepAxis] == out[stepAxis];

    //-- update distance to next crossing
    nextCrossing[stepAxis] += deltaT[stepAxis];

    stop = stop1 || stop2;
  }

  hit_out[0] = hit;
}

int main(){

  RT_Grid *grid; 
  RT_Ray *ray;
  int *hit_out;
  const int     cmpToAxis[8] = {2, 1, 2, 1, 2, 2, 0, 0};

#pragma fast hw_kernel:kernel_IsPointInsidePolygon
  kernel_Intersect(grid, ray, cmpToAxis, hit_out);

}


