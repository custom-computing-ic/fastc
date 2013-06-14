#include "../../../include/maxcc.h"

const int N = 10;
#pragma fast var:y ioType:float(8, 24) computeType:float(8,12) func:kernel_Adp
void kernel_Adp(
                float y, float beta,
                float* prior_m[10],
                float* prior_v[10],
                float* post_m[10],
                float* post_s[10])
{
  float m, s;
  m = 0;
  s = 0;
  for (int i = 0; i < N ; i++) {
    m = m + prior_m[i][0];
    s = s + prior_v[i][0];
  }

  float S = sqrt(beta * beta + s);
  float t = (y * m) / S;

  for (int i = 0; i < N; i++) {
    float pr_m = prior_m[i][0];
    float pr_v = prior_v[i][0];
    // fcall
    // fcall
    float ps_m = pr_m; // TODO
    float ps_s = pr_v; // TODO
    post_m[i][0] = ps_m;
    post_s[i][0] = ps_s;
  }

}
