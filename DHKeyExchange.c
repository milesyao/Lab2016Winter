//simulation of "Diffie–Hellman key exchange". 
/*
Alice and Bob agree to use a modulus p = 23 and base g = 5 (which is a primitive root modulo 23).
Alice chooses a secret integer a = 6, then sends Bob A = g^a mod p
A = 5^6 mod 23 = 8
Bob chooses a secret integer b = 15, then sends Alice B = g^b mod p
B = 5^15 mod 23 = 19
Alice computes s = B^a mod p
s = 19^6 mod 23 = 2
Bob computes s = A^b mod p
s = 8^15 mod 23 = 2
Alice and Bob now share a secret (the number 2).
*/

#include <pbc.h>
#include <pbc_test.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int verbose;
int ifsize;
pairing_t pairing;
element_t g;

void UEActivity(unsigned char **da);
void BSActivity(unsigned char *da);
void usage();

int main(int argc, char **argv) {
  verbose = 0;
  ifsize = 0;
  int canrun =0;
  int choose;
  int k;
  clock_t start_t, end_t;
  float total_t;
  int user_num = 100;

  while ((choose = getopt (argc, argv, "shvn:g")) != -1) {
    switch (choose) {
      case 's':
        ifsize = 1;
        break;
      case 'h':
        usage();
        exit(0);
        break;
      case 'v':
        verbose = 1;
        break;
      case 'n':
        user_num = atoi(optarg);
        break;
      case 'g':
        //printf("Initializing pairing parameters...\n");
        if(canrun) {
          fprintf(stderr, "Pairing parameters have been set, \'-g\' should not set paring parameters again.\n");
          break;
        }
        canrun = 1;
        k=0;
        for( ; optind<argc && !(*argv[optind] == '-'); optind++) k++;
        if(k==2) {
          int rbits = atoi(argv[optind-k]);
          int qbits = atoi(argv[optind-k+1]);
          pbc_param_t param;

          // printf("rbits=%d qbits=%d\n",rbits,qbits);

          pbc_param_init_a_gen(param, rbits, qbits);
          pairing_init_pbc_param(pairing, param);

          pbc_param_clear(param);
        } else {
          fprintf(stderr, "Input invalid!\n");
          usage();
          exit(-1);
        }
        break;
        case '?':
          fprintf(stderr, "Invalid parameters!\n");
          usage();
          exit(-1);
          break;
        default:
          abort();
    }
  }
  if(!canrun) {
    printf("Please at least set \'-g\'\n");
    usage();
    exit(-1);
  }
  //printf("Initializing system variable and public key....\n");
  element_init_G2(g, pairing);
  element_random(g);

  unsigned char *a;

  clock_t tmp_start, tmp;
  for(int i=0; i<user_num; i++) {
	  UEActivity(&a);
	  tmp_start = clock();
	  BSActivity(a);  
	  tmp = clock() - tmp_start;
	  printf("Processing time for this user is %f ms \n",(float)tmp*1000 / CLOCKS_PER_SEC);
  }


  element_clear(g);

  return 0;
}

void usage() {
	printf("****************USAGE***************\n \
-v: Turn on verbose mode. Print signature details.\n \
-h: Help info. \n \
-n: Number of users of the signature generation & verification process. 100 by default. \n \
-g [rbits] [qbits]: Set pairing parameter info by rbits and qbits. The group order r is rbits long, and the order of \
the base field q is qbits long. Elements take qbits to represent.\n \
-s: once used, the memory used by BS will be printed (in bytes). ");

return;
}

void UEActivity(unsigned char **dgen) {
	element_t a, s;
	element_init_Zr(a, pairing); // Alice generate a secret integer
	element_init_G2(s, pairing); // s is the element Alice should send to Bob

	element_random(a);
	element_pow_zn(s, g, a);
	if(verbose) element_printf("Alice send to Bob: %B\n", s);

	int n = pairing_length_in_bytes_compressed_G2(pairing);
	*dgen = pbc_malloc(n);
	element_to_bytes_compressed(*dgen, s);

	element_clear(a);
	element_clear(s);

	return;
}

void BSActivity(unsigned char *dgen) {
	element_t r, b, final;
	element_init_G2(r, pairing);
	element_init_G2(final, pairing);
	element_from_bytes_compressed(r, dgen);
	element_init_G2(b, pairing); // Bob generate a secret integer
	
	element_random(b);
	element_pow_zn(final, r, b);
	if(verbose) element_printf("Bob generates the shared secret: %B \n", final);
	if(ifsize) {
		int totalsize = sizeof(r) + sizeof(b) + sizeof(final);
		element_printf("Memory used at base station is %d bytes. \n", totalsize);
	}
	element_clear(r);
	element_clear(b);
	element_clear(final);
	pbc_free(dgen);

	return;

}

