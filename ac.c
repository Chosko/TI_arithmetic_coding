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
unsigned char atoprob(char *);

// converts arg into sourcelen
unsigned char atosourcelen(char *);

void emit_last_symbols(double, unsigned char *, int);



// the probability that a symbol takes the value 0 (context 1)
double p01;

// the estimated probability (context 1)
double pe0;

// the probability that a symbol takes the value 0 (context 2)
double p02;

// the estimated probability (context 2)
double pe;

// the length of the source stream
int sourcelen;

// verbose boolean
unsigned char verbose;

// the left bound of the interval
double a;

// the right bound of the interval
double b;



void emit_last_symbols(double a, unsigned char * emitted_symbols, int length)
{
  int symbols = floor(a * pow(2, length));
  int i=0;
  unsigned char one = 1;

  for (i = 0; i < length; ++i)
  {
    emitted_symbols[length - i - 1] = (symbols >> i) & one;
  }
}

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

unsigned char atoprob1(char *arg)
{
  p01 = (double)atoi(arg);
  if (p01 <= MAX_P && p01 >= 0)
  {
    p01 /= 100.0;
    return 1;
  }
  else
  {
    print_usage();
    return 0;
  }
}

unsigned char atoprob2(char *arg)
{
  p02 = (double)atoi(arg);
  if (p02 <= MAX_P && p02 >= 0)
  {
    p02 /= 100.0;
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
  printf("USAGE: ac [-v] [<p01> [<sourcelen> [<p02>]]]]\n");
  printf("  Executes the arithmetic coding of a source that generates a random stream of binary symbols\n");
  printf("  Default parameters are: <p01> = %d; <sourcelen> = %d; <p02> = <p01>\n", DEFAULT_P, DEFAULT_SOURCELEN);
  printf("PARAMETERS:\n");
  printf("  -v\t\tverbose\n");
  printf("  <p01>\t\tthe probability (in percent) that a symbol takes the value 0, in context 1\n");
  printf("  <p02>\t\tthe probability (in percent) that a symbol takes the value 0, in context 2\n");
  printf("  <sourcelen>\tthe length of the source stream\n");
  printf("CONSTRAINTS:\n");
  printf("  <p01>\t\tmust be an integer between 0 and %d\n", MAX_P);
  printf("  <p02>\t\tmust be an integer between 0 and %d\n", MAX_P);
  printf("  <sourcelen>\tmust be an integer between 1 and %d\n", MAX_SOURCELEN);
}

unsigned char get_args(int argc, char *argv[])
{
  if (argc == 1);
  else if(argc <= 5)
  {
    int cur_arg = 1;
    if(atoverbose(argv[cur_arg])) cur_arg++;
    if(cur_arg < argc)
    {
      if(!atoprob1(argv[cur_arg])) return 0;
      if(++cur_arg < argc)
        if(!atosourcelen(argv[cur_arg])) return 0;
      if(++cur_arg < argc)
        if(!atoprob2(argv[cur_arg])) return 0;
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
  return ((double)(rand()%1000)) / 1000 >= p01;
}

int main(int argc, char *argv[])
{
  // Initializes variables
  p01 = DEFAULT_P/100.0;
  p02 = -1;
  sourcelen = DEFAULT_SOURCELEN;
  a = 0;
  b = 1;
  pe = 0.5;
  srand(time(NULL));
  if(!get_args(argc, argv)) return 0;
  if(p02 == -1) p02 = p01;
  if(verbose) printf("ac invoked with: p01 = %lf, sourcelen = %d, p02 = %lf\n", p01, sourcelen, p02);

  int i;
  int r1count = 0;
  int r2count = 0;
  int cnt0 = 0;
  int cntTot = 0;
  unsigned char cursym;
  unsigned char underflow;
  unsigned char inaccurate = 0;
  unsigned char outword[sourcelen + 100];
  int outcount = 0;
  double splitpoint;

  printf("Source:\n");
  for (i = 0; i < sourcelen; ++i)
  {
    underflow = 0;
    cursym = emit_source_symbol(); //the current symbol emitted by the source
    splitpoint = a + pe * (b-a);  //the split point between the left and the right sides of the interval
    if (!cursym) cnt0 ++;
    cntTot++;

    
    // Underflow detection
    if(pe != 1 && (splitpoint == a || splitpoint == b))
    {
      underflow = 1;
      inaccurate = 1;
    }

    // interval update
    if (cursym)
      a = splitpoint;
    else
      b = splitpoint;

    // verbose print
    if (verbose) printf("\n----- i=%d -----\n", i);
    if (verbose) printf("Source symbol: ");
    printf("%d", cursym);
    if (verbose) printf("\nEstimated probability of Zero: %lf\n", pe);
    if (verbose) printf("Interval set:\na = %lf\nb = %lf\n", a, b);
    if (verbose && underflow) printf("UNDERFLOW DETECTED!\n");

    // interval renormalization
    if(b < 0.5) //R1
    {
      a *= 2;
      b *= 2;
      if (verbose) printf("Interval renormalized!\n");
      r1count++;
      outword[outcount++] = 0;
    }
    else if (a > 0.5) //R2
    {
      a = 2*(a-0.5);
      b = 2*(b-0.5);
      if (verbose) printf("Interval renormalized!\n");
      r2count++;
      outword[outcount++] = 1;
    }

    // probability estimation update
    pe = (1+(double)cnt0)/(2+cntTot);
  }

  // estimates the length of the last encoded symbols, since last renormalization
  int lastemitted_length = ceil(log2(1/(b-a))) + 1;

  // calculates the estimation of the length of the encoded word
  int length_estimation = lastemitted_length + r1count + r2count;

  // calculates the coderate
  float coderate = (float)length_estimation / sourcelen;

  // calculates the theoric entropy
  double theoric_entropy = (-p01*log2(p01) - (1-p01)*log2(1-p01)) * 1/2 + (-p02*log2(p02) - (1-p02)*log2(1-p02)) * 1/2;
  
  // includes the last emitted symbols into the encoded word
  unsigned char lastemitted_symbols[lastemitted_length];
  emit_last_symbols(a, lastemitted_symbols, lastemitted_length);
  for(i=0; i<lastemitted_length; i++)
  {
    outword[outcount++] = lastemitted_symbols[i];
  }

  //prints output
  printf("\nEncoded word (%d): ", outcount);
  for (i = 0; i < outcount; ++i)
  {
    printf("%d", outword[i]);
  }
  printf("\nEncoded word length: %d bit\n", length_estimation);
  printf("Code rate: %f bit/symbol\n", coderate);
  printf("Theoric source entropy: %lf\n", theoric_entropy);
  if(inaccurate) printf("WARNING - result may be inaccurate: underflow detected!\n");

  return 0;
}