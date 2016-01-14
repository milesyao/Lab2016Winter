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

static inline void pbc_single_pairing_init(pairing_t pairing, int argc, char *argv);

int main(int argc, char **argv) {
  pairing_t pairing;
  pairing_t pairing2;
  element_t g;
  element_t X, Y, a, b, c, cu, r, x, y, A, B, C;
  element_t ax, a1cuxy;
  element_t xy, cuxy;

  pbc_single_pairing_init(pairing, argc, argv[1]);
  pbc_single_pairing_init(pairing2, argc, argv[2]);

  element_init_G2(g, pairing);
  element_init_G2(X, pairing);
  element_init_G2(Y, pairing);
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
  element_init_Zr(x, pairing2);
  element_init_Zr(y, pairing2);
  element_init_Zr(cu, pairing2);

  //generating inputs: 
  //public key: X, Y; signature: A, B, C
  element_random(x);
  element_random(y);
  element_random(g);
  element_printf("system parameter g = %B\n", g);
  //temporarily regard cu as a random number in Zr
  //instead of Cu = r^k&^ru
  element_random(cu);
  element_pow_zn(X, g, x);
  element_pow_zn(Y, g, y);
  element_random(a);
  element_printf("sig component a = %B\n", a);
  element_pow_zn(b, a, y);
  element_printf("sig component b = %B\n", b);
  element_pow_zn(ax, a, x);
  element_mul(xy, x, y);
  element_mul(cuxy, xy, cu);
  element_pow_zn(a1cuxy, a, cuxy);
  element_mul(c, ax, a1cuxy);
  element_printf("sig component c = %B\n", c);

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
  element_clear(x);
  element_clear(y);

  //to do: signature compress & decompress
  
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

  element_clear(exbcu);
  element_clear(tmp1);
  element_clear(tmp2);
  element_clear(right);
  element_clear(left);
  element_clear(A);
  element_clear(B);
  element_clear(C);
  element_clear(X);
  element_clear(Y);
  element_clear(g);
  element_clear(cu);

  return 0;

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


