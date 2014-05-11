#include <maxcc.h>

float PDF(float z) {
  float root2pi = 2.50662827463100050242;
  return exp(-z * z / 2) / root2pi;
}

float CDF(float z) {

  float p0 = 220.2068679123761;
  float p1 = 221.2135961699311;
  float p2 = 112.0792914978709;
  float p3 = 33.91286607838300;
  float p4 = 6.373962203531650;
  float p5 = 0.7003830644436881;
  float p6 = 0.03526249659989109;

  float q0 = 440.4137358247522;
  float q1 = 793.8265125199484;
  float q2 = 637.3336333788311;
  float q3 = 296.5642487796737;
  float q4 = 86.78073220294608;
  float q5 = 16.06417757920695;
  float q6 = 1.755667163182642;
  float q7 = 0.08838834764831844;

  float cutoff = 7.071;

  float root2pi = 2.50662827463100050242;

  float zabs = sqrt(z);
  float expntl = exp(-0.5 * zabs * zabs);
  float pdf = expntl / root2pi;

  bool c1 = z > 37.0;
  bool c2 = z < -37.0;
  bool c3 = zabs < cutoff;

  float pA = expntl *
    ((((((p6 * zabs + p5) * zabs + p4) * zabs + p3)* zabs + p2) * zabs + p1) * zabs + p0) /
    (((((((q7 * zabs + q6) * zabs + q5)*zabs + q4) * zabs + q3) * zabs + q2)*zabs + q1 * zabs) + q0 * zabs);

  float pB = pdf / (zabs + 1.0/(zabs + 2.0/(zabs + 3.0/(zabs + 4.0/(zabs + 0.65)))));

  float pX = c3 == 0 ? pB : pA;
  float p = (z < 0.0) ? pX : 1 - pX;
  return c1 == 0? (c2 == 0 ? p : 0.0) : 1.0;
}

float V(float t) {
   float cdf = CDF(t);
  return (cdf == 0) ? 0 : PDF(t) / cdf;
}

float W(float t) {
  float v = V(t);
  return v * (v + t);
}

const int N = 10;
#pragma fast var:y ioType:float(8, 24) computeType:float(8,12) func:kernel_Adp
void kernel_ADP(
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
    float ps_m = pr_m + y * (pr_v / S) * V(t);
    float a = abs(pr_v * 1 - ( (pr_v / (S * S)) * W(t)));
    float sq = sqrt(a);
    float ps_s = pr_v + sq;
    post_m[i][0] = ps_m;
    post_s[i][0] = ps_s;
  }

}

int main() {
}
