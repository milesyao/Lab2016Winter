README 

This is an evaluation tool for paper "Revisiting Consistency-Availability Tradeoff in Cellular
Mobility Support", especially focusing on the "Access Control with Unclonable Group Identification" part. 

This program requires gmp-6.1.0 and pbc-0.5.14 to run. 

USAGE
=====

Access Control with Unclonable Group Identification
---------------------------------------------------
-v: Turn on verbose mode. Print signature details.
-h: Display help info. 
-f [FILE1][FILE2]: Set pairing parameter info from files on disk. FILE1 and FILE2 stores pairing parameters.
-n: Number of users of the signature generation & verification process. 100 by default.
-g [rbits] [qbits]: Set pairing parameter info by rbits and qbits. The group order r is rbits long, and the order of 
the base field q is qbits long. Elements take qbits to represent.

Diffie-Hellman key exchange
---------------------------
-v: Turn on verbose mode. Print signature details.
-h: Help info. 
-n: Number of users of the signature generation & verification process. 100 by default. 
-g [rbits] [qbits]: Set pairing parameter info by rbits and qbits. The group order r is rbits long, and the order of 
the base field q is qbits long. Elements take qbits to represent.
-s: once used, the memory used by BS will be printed (in bytes). 

****************ATTENTION*******************
1. -g or -f can only be used once per command. 
2. For -f, example parameter files are stored under ./param/*
3. For -g, you must set both "rbits" "qbits" to let program run normally.
4. Once run successfully, you will see the following:
======================
User number: X. 
Total Generation & verification time taken by CPU: 0.019844 seconds.
Total verification time at base station taken by CPU: 0.002325 seconds.
Exiting of the program...
======================

Chunnan Yao
<<<<<<< HEAD:README.md
2015.1.19 revised
=======
2015.1.19 revised


>>>>>>> a23ddd0e40bbf21e5d0abd46186578a16717fb88:README
