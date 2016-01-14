//
//  main.c
//  ACtrlGroupIden
//
//  Created by cnyao on 1/12/16.
//  Copyright © 2016 cnyao. All rights reserved.
//

// Boneh-Lynn-Shacham short signatures demo.
//
// See the PBC_sig library for a practical implementation.
//
// Ben Lynn

// Let me clarify it: we haven't implement step 1 or 2, 
// because they are currently not planned for evaluation
//  (they are bootstrap steps and not bottleneck). Instead, 
//  you only need to implement and evaluate step 3 
//  (prove and verify for access control). 

// Note that to implement step 3, 
 // you may need to assume some parameters (e.g., you assume the certificate, 
 //    random numbers and public/private keys are given as input). 
 // You can write them as input of a function. 


#include <pbc.h>
#include <pbc_test.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>


//system variable & public keys shared
element_t g, X, Y, x, y;
pairing_t pairing;
pairing_t pairing2;
int verbose;

static inline void pbc_single_pairing_init(pairing_t pairing, int argc, char *argv);
void UEActivity(unsigned char **da, unsigned char **db, unsigned char **dc, unsigned char **dcu);
void BSActivity(unsigned char *da, unsigned char *db, unsigned char *dc, unsigned char *dcu);


int main(int argc, char **argv) {
  verbose = 0;
  int k;
  int choose;
  char *para1, *para2;
  while ((choose = getopt (argc, argv, "vf")) != -1) {
    switch (choose) {
      case 'v':
        verbose = 1;
        break;
      case 'f':
        k=0;
        for( ; optind<argc && !(*argv[optind] == '-'); optind++) k++;
        if(k==2) {
          pbc_single_pairing_init(pairing, argc, argv[optind-k]);
          pbc_single_pairing_init(pairing2, argc, argv[optind-k+1]);
        } 
        else if(k==0) {
          //to do: generate param using pbc_param_init_a1_gen
          mpz_t rand_Num, prime;
          unsigned long int i, seed;
          gmp_randstate_t r_state;
          pbc_param_t param;

          seed = 12345678;
          gmp_randinit_default (r_state);
          gmp_randseed_ui(r_state, seed);

          mpz_init(rand_Num);
          mpz_init(prime);
          mpz_urandomb(rand_Num,r_state,200);

          mpz_nextprime(prime, rand_Num);
          pbc_param_init_a1_gen(param, prime);
          pairing_init_pbc_param(pairing, param);

          mpz_nextprime(prime, rand_Num);
          pbc_param_init_a1_gen(param, prime);
          pairing_init_pbc_param(pairing2, param);

          gmp_randclear(r_state);
          mpz_clear(rand_Num);
          mpz_clear(prime);
          pbc_param_clear(param);
        } else {
          fprintf(stderr, "Input invalid!\n");
          exit(-1);
        }
        break;
        default:
          abort();
    }
  }

  printf("Initializing system variable and public key....\n");

  element_init_G2(g, pairing);
  element_init_G2(X, pairing);
  element_init_G2(Y, pairing);
  element_init_Zr(x, pairing2);
  element_init_Zr(y, pairing2);

  element_random(x);
  element_random(y);

  //system variable & public key generation
  element_random(g);
  if(verbose) element_printf("system parameter g = %B\n", g);
  element_pow_zn(X, g, x);
  element_pow_zn(Y, g, y);

  unsigned char *a, *b, *c, *cu;

  /*******Working********/
  printf("New user comes...\n");
  UEActivity(&a, &b, &c, &cu);
  BSActivity(a, b, c, cu);

  printf("************************\n");

  printf("New user comes...\n");
  UEActivity(&a, &b, &c, &cu);
  BSActivity(a, b, c, cu);


  element_clear(g);
  element_clear(X);
  element_clear(Y);
  element_clear(x);
  element_clear(y);

  return 0;
}

void UEActivity(unsigned char **da, unsigned char **db, unsigned char **dc, unsigned char **dcu) {
  printf("Generating keys.....\n");

  element_t a, b, c, cu, r, A, B, C;
  element_t ax, a1cuxy;
  element_t xy, cuxy;

  element_init_G1(a, pairing);
  element_init_G1(b, pairing);
  element_init_G1(c, pairing);
  element_init_Zr(r, pairing);
  element_init_G1(A, pairing);
  element_init_G1(B, pairing);
  element_init_G1(C, pairing);

  element_init_G1(ax, pairing);
  element_init_G1(a1cuxy, pairing);

  //temporarily regard p and q are independent
  //instead of p = 2q ＋ 1
  element_init_Zr(xy, pairing2);
  element_init_Zr(cuxy, pairing2);
  element_init_Zr(cu, pairing2);

  //temporarily regard cu as a random number in Zr
  //instead of Cu = r^k&^ru
  element_random(cu);
  element_random(a);
  if(verbose) element_printf("sig component a = %B\n", a);
  element_pow_zn(b, a, y);
  if(verbose) element_printf("sig component b = %B\n", b);
  element_pow_zn(ax, a, x);
  element_mul(xy, x, y);
  element_mul(cuxy, xy, cu);
  element_pow_zn(a1cuxy, a, cuxy);
  element_mul(c, ax, a1cuxy);
  if(verbose) element_printf("sig component c = %B\n", c);

  //blind the signature
  element_random(r);
  element_pow_zn(A, a, r);
  element_pow_zn(B, b, r);
  element_pow_zn(C, c, r);
  
  //clear meta elements
  element_clear(ax);
  element_clear(a1cuxy);
  element_clear(xy);
  element_clear(cuxy);
  element_clear(r);
  element_clear(a);
  element_clear(b);
  element_clear(c);

  //signature compress
  int n = pairing_length_in_bytes_compressed_G1(pairing);
  int m = pairing_length_in_bytes_Zr(pairing2);
  *da = pbc_malloc(n);
  *db = pbc_malloc(n);
  *dc = pbc_malloc(n);
  *dcu = pbc_malloc(m);
  element_to_bytes_compressed(*da, A);
  element_to_bytes_compressed(*db, B);
  element_to_bytes_compressed(*dc, C);
  element_to_bytes(*dcu, cu);

  return;
}

void BSActivity(unsigned char *da, unsigned char *db, unsigned char *dc, unsigned char *dcu) {
  //signature decompress
  printf("Verifying....\n");

  element_t cu, A, B, C;
  element_init_G1(A, pairing);
  element_init_G1(B, pairing);
  element_init_G1(C, pairing);
  element_init_Zr(cu, pairing2);
  
  element_from_bytes_compressed(A, da);
  element_from_bytes_compressed(B, db);
  element_from_bytes_compressed(C, dc);
  element_from_bytes(cu, dcu);
 
  //verification I
  element_t exbcu;
  element_t tmp1, tmp2, right, left;
  element_init_GT(exbcu, pairing);
  element_init_GT(tmp1, pairing);
  element_init_GT(tmp2, pairing);
  element_init_GT(right, pairing);
  element_init_GT(left, pairing);

  element_pairing(tmp1, X, A);
  element_pairing(tmp2, X, B);
  element_pow_zn(exbcu, tmp2, cu);
  element_mul(left, tmp1, exbcu);
  element_pairing(right, g, C);

  if (!element_cmp(left, right)) {
          printf("part 1 verifies\n");
  } else {
      printf("*BUG* part 1 does not verify *BUG*\n");
  }

  //verification II
  element_pairing(left, A, Y);
  element_pairing(right, g, B);

  if (!element_cmp(left, right)) {
          printf("part 2 verifies\n");
  } else {
      printf("*BUG* part 2 does not verify *BUG*\n");
  }

  pbc_free(da);
  pbc_free(db);
  pbc_free(dc);
  pbc_free(dcu);

  element_clear(exbcu);
  element_clear(tmp1);
  element_clear(tmp2);
  element_clear(right);
  element_clear(left);
  element_clear(A);
  element_clear(B);
  element_clear(C);
  element_clear(cu);

  return;
}


static inline void pbc_single_pairing_init(pairing_t pairing, int argc, char *argv) {
  char s[16384];
  FILE *fp = stdin;

  if (argc > 1) {
    fp = fopen(argv, "r");
    if (!fp) pbc_die("error opening %s", argv);
  }
  size_t count = fread(s, 1, 16384, fp);
  if (!count) pbc_die("input error");
  fclose(fp);

  if (pairing_init_set_buf(pairing, s, count)) pbc_die("pairing init failed");
}


