int   IsPointInsidePolygon(const RT_Polygon *polygon, const RT_Point *p)
{
  int       i0 = polygon->projectionDimension[0];
  int       i1 = polygon->projectionDimension[1];
  unsigned int  n;
  RT_Point    e0, e1;
  int       y0, y1;
  int       inside = RT_FALSE;


  for(n=1; n<=polygon->nVertices; n++)
  {
    y1 = (e1.c[i1] >= p->c[i1]) ? RT_TRUE : RT_FALSE;
    if (y0 != y1)
      if (( (e1.c[i1]-p->c[i1]) * (e0.c[i0]-e1.c[i0]) >= (e1.c[i0]-p->c[i0]) * (e0.c[i1]-e1.c[i1]) ) == y1)
        inside = (inside == RT_TRUE) ? RT_FALSE : RT_TRUE;
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
  
  void    InitPointWithPoint(RT_Point *p, const RT_Point *withPoint)
  {
    (*p).c[0] = (*withPoint).c[0];
    (*p).c[1] = (*withPoint).c[1];
    (*p).c[2] = (*withPoint).c[2];
  }

  return inside;
}


int Intersect(RT_Grid *grid, RT_Ray *ray, RT_Intersection *intersection) {
  float       rayT;
  RT_Point      p, gridIntersect;
  RT_Intersection   localIntersection;
  float       nextCrossing[3], deltaT[3];
  int         step[3], out[3], pos[3], axis, stepAxis, bits;
  int         hit = RT_FALSE;
  RT_Voxel      *voxel;
  const int     cmpToAxis[8] = {2, 1, 2, 1, 2, 2, 0, 0};

  //------------------------------------------------------------------------------
  // Traverse the voxels along the ray
  //------------------------------------------------------------------------------
  for(;;) {
    //------------------------------------------------------------------------------
    // Fetch the current voxel
    //------------------------------------------------------------------------------
    voxel = grid->voxels[(pos[2] * grid->size[1]+y)* grid->size[0]+x];

    //------------------------------------------------------------------------------
    // Compute the intersection of the ray with the primitives belonging to the
    //   voxel (if any)
    //------------------------------------------------------------------------------
    if (voxel != NULL)
      if (IntersectSingleVoxel(voxel, ray, intersection))
        hit = RT_TRUE;

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


    //------------------------------------------------------------------------------
    // Step along the voxel grid
    //------------------------------------------------------------------------------
    //-- find the closest voxel limit and its corresponding axis X, Y or Z
    bits = ((nextCrossing[0] < nextCrossing[1])<<2) + ((nextCrossing[0] < nextCrossing[2])<<1) + ((nextCrossing[1] <              nextCrossing[2]));
    stepAxis = cmpToAxis[bits];

    //-- ray section out of the voxel 
    if (ray->d_out < nextCrossing[stepAxis])
      break;

    //-- go to next voxel
    pos[stepAxis] += step[stepAxis];

    //-- exit if out of voxel grid
    if (pos[stepAxis] == out[stepAxis])
      break;

    //-- update distance to next crossing
    nextCrossing[stepAxis] += deltaT[stepAxis];
  }

  return hit;
}
