#include <stdio.h>
#include <math.h>
#define MAX_BLOCKLEN 32
#define MAX_P 100


// prints the program usage instructions
void print_usage();

// emits a binary symbol (bit) with P(X=0) = p
unsigned char emit_source_symbol(double p);

// converts arg into probability
double atoprob(char *arg);

// converts arg into blocklen
int atoblocklen(char *arg);

int atoblocklen(char *arg)
{
  int blocklen = atoi(arg);
  if (blocklen >= 1 && blocklen <= MAX_BLOCKLEN)
    return blocklen;
  else
  {
    print_usage();
    return -1;
  }
}

double atoprob(char *arg)
{
  double p0 = (double)atoi(arg);
  if (p0 <= MAX_P && p0 >= 0)
    return p0/100.0;
  else
  {
    print_usage();
    return -1;
  }
}

void print_usage()
{
  printf("USAGE: ac [-v] [<p0> [<blocklen>]]\n");
  printf("  Executes the arithmetic coding of a source that generates random blocks of binary symbols\n");
  printf("  Default parameters are: <p0> = 0.5; <blocklen> = 8\n");
  printf("PARAMETERS:\n");
  printf("  -v\t\tverbose\n");
  printf("  <p0>\t\tthe probability (in percent) that a symbol takes the value 0\n");
  printf("  <blocklen>\tthe length of a block of symbols\n");
  printf("CONSTRAINTS:\n");
  printf("  <p0>\t\tmust be an integer between 0 and %d\n", MAX_P);
  printf("  <blocklen>\tmust be an integer between 1 and %d\n", MAX_BLOCKLEN);
}


unsigned char emit_source_symbol(double p)
{
  return ((double)(rand()%1000)) / 1000 >= p;
}

int main(int argc, char *argv[])
{
  double p0 = 0.5;
  unsigned char blocklen = 8;
  unsigned char verbose = 0;
  if (argc == 1);
  else if (argc == 2)
  {
    if(argv[1][0] == '-' && argv[1][1] == 'v' && argv[1][2] == 0)
    {
      verbose = 1;
    }
    else
    {
      p0 = atoprob(argv[1]);
      if (p0 == -1) return 0;
    }
  }
  else if (argc == 3)
  {
    if(argv[1][0] == '-' && argv[1][1] == 'v' && argv[1][2] == 0)
    {
      verbose = 1;
      p0 = atoprob(argv[2]);
      if (p0 == -1) return 0;
    }
    else
    {
      p0 = atoprob(argv[1]);
      if (p0 == -1) return 0;
      int int_blocklen = atoblocklen(argv[2]);
      if(int_blocklen == -1) return 0;
      else blocklen = (unsigned char) int_blocklen;
    }
  }
  else if (argc == 4)
  {
    if(argv[1][0] == '-' && argv[1][1] == 'v' && argv[1][2] == 0)
    {
      verbose = 1;
      p0 = atoprob(argv[2]);
      if (p0 == -1) return 0;
      int int_blocklen = atoblocklen(argv[3]);
      if(int_blocklen == -1) return 0;
      else blocklen = (unsigned char) int_blocklen;
    }
    else
    {
      print_usage();
      return 0;
    }
  }
  else
  {
    print_usage();
    return 0;
  }

  return 0;
}