#include <stdio.h>
#include <math.h>
#define MAX_SOURCELEN 100000
#define MAX_P 100


// prints the program usage instructions
void print_usage();

// emits a binary symbol (bit) with P(X=0) = p0
unsigned char emit_source_symbol();

// converts arg into probability
unsigned char atoprob(char *arg);

// converts arg into sourcelen
unsigned char atosourcelen(char *arg);

// the probability that a symbol takes the value 0
double p0;

// the length of the source stream
int sourcelen;

// verbose boolean
unsigned char verbose;

unsigned char atosourcelen(char *arg)
{
  sourcelen = atoi(arg);
  if (sourcelen >= 1 && sourcelen <= MAX_SOURCELEN)
    return 1;
  else
  {
    print_usage();
    return 0;
  }
}

unsigned char atoprob(char *arg)
{
  p0 = (double)atoi(arg);
  if (p0 <= MAX_P && p0 >= 0)
  {
    p0 /= 100.0;
    return 1;
  }
  else
  {
    print_usage();
    return 0;
  }
}

unsigned char atoverbose(char *arg)
{
  if(arg[0] == '-' && arg[1] == 'v' && arg[2] == 0)
  {
    verbose = 1;
    return 1;
  }
  else
  {
    verbose = 0;
    return 0;
  }
}

void print_usage()
{
  printf("USAGE: ac [-v] [<p0> [<sourcelen>]]\n");
  printf("  Executes the arithmetic coding of a source that generates a random stream of binary symbols\n");
  printf("  Default parameters are: <p0> = 0.5; <sourcelen> = 32\n");
  printf("PARAMETERS:\n");
  printf("  -v\t\tverbose\n");
  printf("  <p0>\t\tthe probability (in percent) that a symbol takes the value 0\n");
  printf("  <sourcelen>\tthe length of the source stream\n");
  printf("CONSTRAINTS:\n");
  printf("  <p0>\t\tmust be an integer between 0 and %d\n", MAX_P);
  printf("  <sourcelen>\tmust be an integer between 1 and %d\n", MAX_SOURCELEN);
}


unsigned char emit_source_symbol()
{
  return ((double)(rand()%1000)) / 1000 >= p0;
}

int main(int argc, char *argv[])
{
  p0 = 0.5;
  sourcelen = 8;

  if (argc == 1);
  else if(argc <= 4)
  {
    int cur_arg = 1;
    if(atoverbose(argv[cur_arg])) cur_arg++;
    if(cur_arg < argc)
    {
      if(!atoprob(argv[cur_arg])) return 0;
      if(++cur_arg < argc)
        if(!atosourcelen(argv[cur_arg])) return 0; 
    }
  }
  else
  {
    print_usage();
    return 0;
  }

  printf("verbose: %d, p0: %lf, sourcelen: %d\n", verbose, p0, sourcelen);

  // int i;
  // for (i = 0; i < sourcelen; ++i)
  // {
  //   printf("%d\n", emit_source_symbol());
  // }
  return 0;
}