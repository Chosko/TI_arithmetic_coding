#include <stdio.h>
#include <math.h>
#define MAX_SOURCELEN 100000
#define MAX_P 100
#define DEFAULT_SOURCELEN 8
#define DEFAULT_P 50

// prints the program usage instructions
void print_usage();

// try to get the args of the program
unsigned char get_args();

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

// the left bound of the interval
double a;

// the right bound of the interval
double b;



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
  printf("  Default parameters are: <p0> = %d; <sourcelen> = %d\n", DEFAULT_P, DEFAULT_SOURCELEN);
  printf("PARAMETERS:\n");
  printf("  -v\t\tverbose\n");
  printf("  <p0>\t\tthe probability (in percent) that a symbol takes the value 0\n");
  printf("  <sourcelen>\tthe length of the source stream\n");
  printf("CONSTRAINTS:\n");
  printf("  <p0>\t\tmust be an integer between 0 and %d\n", MAX_P);
  printf("  <sourcelen>\tmust be an integer between 1 and %d\n", MAX_SOURCELEN);
}

unsigned char get_args(int argc, char *argv[])
{
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
  return 1;
}

unsigned char emit_source_symbol()
{
  return ((double)(rand()%1000)) / 1000 >= p0;
}

int main(int argc, char *argv[])
{
  // Initializes variables
  p0 = DEFAULT_P/100.0;
  sourcelen = DEFAULT_SOURCELEN;
  a = 0;
  b = 1;
  srand(time(NULL));
  if(!get_args(argc, argv)) return 0;
  if(verbose) printf("ac invoked with: p0 = %lf, sourcelen = %d\n", p0, sourcelen);

  int i;
  unsigned char cursym;
  double splitpoint;
  printf("Source:\n");
  for (i = 0; i < sourcelen; ++i)
  {
    cursym = emit_source_symbol(); //the current symbol emitted by the source
    splitpoint = a + p0 * (b-a);  //the split point between the left and the right sides of the interval

    // interval update
    if (cursym)
      b = splitpoint;
    else
      a = splitpoint;

    if (verbose) printf("\n----- i=%d -----\n", i);
    if (verbose) printf("Source symbol: ");
    printf("%d", cursym);
    if (verbose) printf("\nInterval set:\na = %lf\nb = %lf\n", a, b);
  }
  printf("\n");

  return 0;
}