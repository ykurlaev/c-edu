#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double b(double x)
{
  return (1-x*x)*exp(-(x*x)/2);
}

double cwt(double *l, int j, double s, double dt, int c)
{
  int k;
  double r = 0;
  for(k=0; k<c-1; k++)
    r += l[k]*(dt/sqrt(s))*b((k-j)*dt/s);
  return r;
}

int main(int argc, char *argv)
{
  static double l[5000];
  double _;
  int i = 0, j, c = 0;
  while(scanf("%lf%lf",&_,&l[i++])==2) c++;
  double ds = 0.001302, dt = 1./768, L = 0.5, s;
  for(j=0; j<c-1; j++)
  {
    for(s=ds; s<L; s+=ds)
      printf("%lf ",cwt(l,j,s,dt,c));
    printf("\n");
  }
  return 0;
}
