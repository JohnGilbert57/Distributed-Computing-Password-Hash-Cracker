/* File:     hash_cracker.c
 * Johnny Gilbert (jg480318)
 * 4/2/2022
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>


int debug = 1;  // must be turned OFF for github grading

const int bufsize=256;


// routine definitions
void Get_input(int my_rank, int comm_sz, char *salt, char *hash, char *alphabet, int *ppwd_len);
void ix_to_password(char *alphabet, int pwd_length, int ix, char *guess);
int FindHash(int my_rank, char *salt, char *hash, char *alphabet, int pwd_length, int first_pwd_ix, int num_pwds);

// You shouldn't need to change anything about this routine
char *MakeSha256( char *salt,  char *pwd_guess) {
   char command[1024];
   char result[1024];
   FILE *file_result;

   // create the shell command to use
   // for example: 
   // BSH:Saru> openssl passwd -5 -salt foobar mypwd
   // $5$foobar$6WS2Np/pNB83FbxzS7a5fGaJO1PMtdldjMSWCiBio05
   snprintf(command, sizeof(command), "openssl passwd -5 -salt '%s' '%s'\n", salt, pwd_guess);

   if (debug>1)
      fprintf(stderr,"Running command %s\n", command);

   // run the shell command
   file_result = popen(command,"r");

   fgets(result, sizeof(result), file_result);

   pclose(file_result);

   // printf("Digest: '%s'", result);
   char *ptr = result;
   ptr=index(result+1,'$');   // find the second $
   ptr=1+index(ptr+1,'$');   // find the third $
   // printf("Hash: '%s'", ptr);

   return(strdup(ptr));
}


int main(void) {
   int my_rank;   // my rank, CPU number
   int comm_sz;   // number of CPUs in the group
   char salt[bufsize];       // the salt used to generate the hash (a string, like "foobar")
   char hash[bufsize];       // the password hash we're trying to find (a string, like "AiVIwiOWe.ZrPkJTJs30soPiP2dYmpbNm8faGkAMBr8")
   char alphabet[bufsize];   // the alphabet that the passwords are taken from (a string, like "0123456789")
   int password_length;       // how many characters (from that alphabet) are in the password
   int ix1;                   // the first password index that _I_ am supposed to start testing
   int ix2;                   // the last password index that _I_ am supposed to test
   int total_passwords = 1;       // how many do we need to check?
   int my_answer_ix;          // the answer that I came up with
   int answer = 0;            // the best answer from anybody

   
   /* Let the system do what it needs to start up MPI */
   MPI_Init(NULL, NULL);

   /* Get my process rank */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   /* Find out how many processes are being used */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   /* Print the status */
   if (debug && my_rank == 0) {
      printf("Comm size: %d\n", comm_sz);
   }

   Get_input(my_rank, comm_sz, salt, hash, alphabet, &password_length);

   /* Print the status */
   if (debug && my_rank == 0) {
      printf("Target hash: '%s'\n", hash);      
      printf("Salt used: '%s'\n", salt);
      printf("Password alphabet: %s\n", alphabet);
      printf("Password Length: %d\n", password_length);
   }

   // how many total passwords do we need to check?
   // you will need to calculate this!
   int alphabet_length = strlen(alphabet);
   for (int i = 0; i < password_length; i++) {
      total_passwords *= alphabet_length;
   }

   // Print the workload
   if (debug && my_rank == 0) {
      printf("Total passwords: %d\n", total_passwords);
      printf("Passwords per rank: %d\n", total_passwords / comm_sz);
   }

   // what work does THIS cpu need to do
   // you will need to calculate this
   int iterations = total_passwords / comm_sz;
   ix1 = my_rank * iterations;    // first password index to check
   if (my_rank == comm_sz - 1){
      ix2 = total_passwords - 1;
   }
   else {
      ix2 = ix1 + iterations - 1;
   }
   // perform the assigned work
   my_answer_ix = FindHash(my_rank, salt, hash, alphabet, password_length, ix1, ix2);

   if (debug)
      printf("CPU %d returns answer %d\n", my_rank, my_answer_ix);
   // gather up the max password ix found */
   // you will need to finish this
   // you need to use MPI_Reduce to find the MAXIMUM value of my_answer_ix of all the CPUs
   MPI_Reduce(&my_answer_ix, &answer, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

   /* Print the result */
   if (my_rank == 0) {
      if (answer >= 0) {
         char guess[bufsize];
         ix_to_password(alphabet, password_length, answer, guess);
         if (debug) {
            printf("Found Answer: %d (%s)\n", answer, guess);
         } else { 
            printf("%d: '%s'\n", answer, guess);
         }
      } else {
            printf("not found\n");
      }
   }
   /* Shut down MPI */
   MPI_Finalize();
   return 0;
} /*  main  */


// you will need to finish this routine
void Get_input(int my_rank, int comm_sz, char *salt, char *hash, char *alphabet, int *ppwd_len) {
   if (my_rank == 0) {
      int ret;
      fprintf(stderr,"Enter salt, hash, alphabet, and passwdlength\n");
      ret = scanf("%s %s %s %d", salt, hash, alphabet, ppwd_len);
      if (ret != 4) {
         fprintf(stderr,"Invalid arguments provided (%d)\n", ret);
         exit(0);
      }
   } 
   // you will need to complete the 4 MPI_Bcast calls needed to share those 4 parameters with everybody...
   MPI_Bcast(salt, bufsize, MPI_CHAR, 0, MPI_COMM_WORLD);
   MPI_Bcast(hash, bufsize, MPI_CHAR, 0, MPI_COMM_WORLD);
   MPI_Bcast(alphabet, bufsize, MPI_CHAR, 0, MPI_COMM_WORLD);
   MPI_Bcast(ppwd_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
} 




// you need to write this routine VERY carefully
// its purpose is to fill in the array "guess" with the ix'th password from the given alphabet
void ix_to_password(char *alphabet, int pwd_length, int ix, char *guess) {
   for(int i = 0; i < pwd_length; i++) {
      guess[pwd_length-i-1] = alphabet[ix%strlen(alphabet)];
      ix /= strlen(alphabet);
   }
   guess[pwd_length] = '\00';  // make sure it's NULL terminated
   printf("guess: %s", guess);
}

// you need to finish this routine
// it needs to loop and generate each password from first_pwd_ix to last_pwd_ix
// for each one, compare against "target_hash" to check for a match
// if it matches, you should immediately return the ix of the matching password
// if none found, return -1
int FindHash(int my_rank, char *salt, char *target_hash, char *alphabet, int pwd_length, int first_pwd_ix, int last_pwd_ix) {
   char guess[pwd_length+2];
   for (int ix=first_pwd_ix; ix <= last_pwd_ix; ++ix) {
      ix_to_password(alphabet, pwd_length, 10619936, guess);
      if(!strncmp(MakeSha256(salt, guess), target_hash, strlen(target_hash))) {
         return ix;
      }
   }
   return(-1);
}