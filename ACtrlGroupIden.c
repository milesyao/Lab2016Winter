//
//  main.c
//  ACtrlGroupIden
//
//  Created by cnyao on 1/12/16.
//  Copyright © 2016 cnyao. All rights reserved.
//

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
#include <time.h>


//system variable & public keys shared
element_t g, X, Y, x, y;
pairing_t pairing;
pairing_t pairing2;
int verbose;
int ifsize;

static inline void pbc_single_pairing_init(pairing_t pairing, int argc, char *argv);
void UEActivity(unsigned char **da, unsigned char **db, unsigned char **dc, unsigned char **dcu);
void BSActivity(unsigned char *da, unsigned char *db, unsigned char *dc, unsigned char *dcu);
void usage();

int main(int argc, char **argv) {
  verbose = 0;
  ifsize = 0;
  int canrun =0;
  clock_t start_t, end_t;
  float total_t;
  int user_num = 100;
  int k;
  int choose;
  char *para1, *para2;
  while ((choose = getopt (argc, argv, "vfn:hgs")) != -1) {
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

          pairing_init_pbc_param(pairing2, param);

          pbc_param_clear(param);
        } else {
          fprintf(stderr, "Input invalid!\n");
          usage();
          exit(-1);
        }
        break;
      case 'f':
        //printf("Initializing pairing parameters...\n");
        if(canrun) {
          fprintf(stderr, "Pairing parameters have been set, \'-f\' should not set paring parameters again.\n");
          break;
        }
        canrun = 1;
        k=0;
        for( ; optind<argc && !(*argv[optind] == '-'); optind++) k++;
        if(k==2) {
          pbc_single_pairing_init(pairing, argc, argv[optind-k]);
          pbc_single_pairing_init(pairing2, argc, argv[optind-k+1]);
        }  
        else {
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
    printf("Please at least set \'-f\' or \'-g\'\n");
    usage();
    exit(-1);
  }

  //printf("Initializing system variable and public key....\n");
  element_init_G2(g, pairing);
  element_init_G2(X, pairing);
  element_init_G2(Y, pairing);
  element_init_Zr(x, pairing2);
  element_init_Zr(y, pairing2);

  element_random(x);
  element_random(y);

  printf("g=%lu X=%lu Y=%lu x=%lu y=%lu\n",sizeof(g),sizeof(X),sizeof(Y),sizeof(x),sizeof(y));

  //system variable & public key generation
  element_random(g);
  if(verbose) element_printf("system parameter g = %B\n", g);
  element_pow_zn(X, g, x);
  element_pow_zn(Y, g, y);
//   mpz_t mpz_g, mpz_X, mpz_Y;
//   mpz_inits(mpz_g, mpz_X, mpz_Y, mpz_x, mpz_y, NULL);
//   element_to_mpz(mpz_g, g);
//   element_to_mpz(mpz_x, x);
//   element_to_mpz(mpz_y, y); 
//   mpz_powm(mpz_X, mpz_g, mpz_x, pairing->r);
//   element_set_mpz(X, mpz_X);
//   //element_pow_zn(X, g, x);
//   mpz_powm(mpz_Y, mpz_g, mpz_y, pairing->r);
//   element_set_mpz(Y, mpz_Y);
//   if(verbose) {
//         gmp_printf("pair order %zd\n", pairing->r);
// 	gmp_printf("mpz g %zd\n", mpz_g);
//         element_printf("x = %B\n", x);
// 	gmp_printf("mpz x %zd\n", mpz_x);
//         gmp_printf("mpz y %zd\n", mpz_y);
//         gmp_printf("mpz X %zd\n", mpz_X);
// 	element_printf("public key X = %B\n", X);
//         element_printf("public key Y = %B\n", Y);
//   }
// //element_pow_zn(Y, g, y);
//   mpz_clear(mpz_g);mpz_clear(mpz_X);mpz_clear(mpz_Y);
  unsigned char *a, *b, *c, *cu;

  /*******Working********/
  start_t = clock();
  clock_t tmp_start;
  clock_t bscurtotal = 0.0;
  float bstotal;
  clock_t tmp=0;
  for(int i=0; i<user_num; i++) {
    //printf("New user comes...\n");
    UEActivity(&a, &b, &c, &cu);
    tmp_start = clock();
    BSActivity(a, b, c, cu);
    tmp = clock() - tmp_start;
    printf("Processing time for this user is %f ms \n",(float)tmp*1000 / CLOCKS_PER_SEC);
    bscurtotal += tmp;
  }

  //printf("************************\n");


  end_t = clock();

  total_t = (float)(end_t - start_t) / CLOCKS_PER_SEC;
  bstotal = (float)bscurtotal / CLOCKS_PER_SEC;
  //printf("User number: %d. \nTotal Generation & verification time taken by CPU: %f seconds.\n", user_num, total_t);
  //printf("Total verification time at base station taken by CPU: %f seconds.\n", bstotal);
  //printf("Exiting of the program...\n");

  element_clear(g);
  element_clear(X);
  element_clear(Y);
  element_clear(x);
  element_clear(y);

  return 0;
}

void UEActivity(unsigned char **da, unsigned char **db, unsigned char **dc, unsigned char **dcu) {
  //printf("Generating keys.....\n");

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
  // mpz_t mpz_a, mpz_b, mpz_c;
  // mpz_inits(mpz_a, mpz_b, mpz_c, NULL);
  // element_to_mpz(mpz_a, a);
  // mpz_powm(mpz_b, mpz_a, mpz_y, pairing->r);
  // element_set_mpz(b, mpz_b);
 //element_pow_zn(b, a, y);
  
  if(verbose) element_printf("sig component b = %B\n", b);
  element_pow_zn(ax, a, x);
  // mpz_t mpz_ax, mpz_a1cuxy, mpz_cuxy;
  // mpz_inits(mpz_ax, mpz_a1cuxy, mpz_cuxy, NULL);
  // mpz_powm(mpz_ax, mpz_a, mpz_x, pairing->r);
  // element_set_mpz(ax, mpz_ax);

  element_mul(xy, x, y);
  element_mul(cuxy, xy, cu);
  element_pow_zn(a1cuxy, a, cuxy);
  // element_to_mpz(mpz_cuxy, cuxy);
  // mpz_powm(mpz_a1cuxy, mpz_a, mpz_cuxy, pairing->r);
  // element_set_mpz(a1cuxy, mpz_a1cuxy);
  element_mul(c, ax, a1cuxy);
  if(verbose) element_printf("sig component c = %B\n", c);

  //blind the signature
  // mpz_t mpz_A, mpz_B, mpz_C, mpz_r;
  // mpz_inits(mpz_A, mpz_B, mpz_C, mpz_r, NULL);
  element_random(r);
  element_pow_zn(A, a, r);
  element_pow_zn(B, b, r);
  element_pow_zn(C, c, r);
  // element_to_mpz(mpz_r, r);
  // mpz_powm(mpz_A, mpz_a, mpz_r, pairing->r);
  // mpz_powm(mpz_B, mpz_b, mpz_r, pairing->r);
  // mpz_powm(mpz_C, mpz_c, mpz_r, pairing->r);
  // element_set_mpz(A, mpz_A);
  // element_set_mpz(B, mpz_B);
  // element_set_mpz(C, mpz_C);  
  
  //clear meta elements
  element_clear(ax);
  element_clear(a1cuxy);
  element_clear(xy);
  element_clear(cuxy);
  element_clear(r);
  element_clear(a);
  element_clear(b);
  element_clear(c);
  // mpz_clear(mpz_a);
  // mpz_clear(mpz_b);
  // mpz_clear(mpz_c);
  // mpz_clear(mpz_ax);
  // mpz_clear(mpz_a1cuxy);
  // mpz_clear(mpz_cuxy);
  // mpz_clear(mpz_A);
  // mpz_clear(mpz_B);
  // mpz_clear(mpz_C);
  // mpz_clear(mpz_r);
  
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
  //printf("Verifying....\n");

  element_t cu, A, B, C;
  element_init_G1(A, pairing);
  element_init_G1(B, pairing);
  element_init_G1(C, pairing);
  element_init_Zr(cu, pairing2);
  
  element_from_bytes_compressed(A, da);
  element_from_bytes_compressed(B, db);
  element_from_bytes_compressed(C, dc);
  element_from_bytes(cu, dcu);

  pbc_free(da);
  pbc_free(db);
  pbc_free(dc);
  pbc_free(dcu);
 
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
          //printf("part 1 verifies\n");
  } else {
      printf("*BUG* part 1 does not verify *BUG*\n");
  }

  //verification II
  element_pairing(left, A, Y);
  element_pairing(right, g, B);

  if (!element_cmp(left, right)) {
          //printf("part 2 verifies\n");
  } else {
      printf("*BUG* part 2 does not verify *BUG*\n");
  }

  if(ifsize) {
    int totalsize = sizeof(tmp1) + sizeof(tmp2) + sizeof(right) + sizeof(left) + 
                    sizeof(A) + sizeof(B) + sizeof(C) + sizeof(cu);
    printf("Memory used at base station is %d bytes. \n", totalsize);

  }

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

  void usage() {
    printf("****************USAGE***************\n \
-v: Turn on verbose mode. Print signature details.\n \
-h: Help info. \n \
-f [FILE1][FILE2]: Set pairing parameter info from files on disk. FILE1 and FILE2 stores pairing parameters.\n \
-n: Number of users of the signature generation & verification process. 100 by default. \n \
-g [rbits] [qbits]: Set pairing parameter info by rbits and qbits. The group order r is rbits long, and the order of \
the base field q is qbits long. Elements take qbits to represent.\n \
-s: once used, the memory used by BS will be printed (in bytes). ");

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


