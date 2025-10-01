#include <stdio.h>

/* This is a simple program to demonstrate the use of Protmem. We set up a Protmem
   protected memory segment (PMS) containing a 4 byte secret value, set up the Protmem
   Instruction Pointer to point to some code which will just copy out this secret
   value into unprotected memory, and enter Protmem locked mode. We first demonstrate
   correct access to the PMS using the enterprot instruction, and then we trigger a
   Protmem fault by attempting to access the PMS directly.
 */

/* tell the assembler to use our Protmem extension */
asm(".option arch, +xprotmem\n\t");

int main(){
  /* The char array secret represents some secret data (e.g. a cryptographic
     key) which we want to protect with the Protmem feature. The char array
     nonsecret is a location to which we shall copy the contents of secret
     while in Protmem unlocked mode (of course, copying the secret data out
     of the protected memory segment would negate the point of using protected
     memory segment in a real application, but we are going for a very simple
     demo here).
   */
  unsigned char secret[4] = {17,96,101,27};
  unsigned char nonsecret[4] = {0,0,0,0};

  /* Set up the Protmem protections. Note that we start in Protmem unlocked mode,
     which allows us to use the setproti and setprotd instructions.
   */
  printf("doing setup...\n");
  asm volatile(
               "la t1,_protmem_func\n\t" // 1| set the Protmem instruction pointer to
               "setproti t1\n\t"         // 1|   point at _protmem_func
               "li t1,4\n\t"             // 2| set the Protmem protected segment to start
               "setprotd %0, t1\n\t"     // 2|   at the beginning of secret and have length 4
               "la t1,_done_protmem_setup\n\t" // 3| move into Protmem unlocked mode by doing
               "exitprot t1\n\t"               // 3|   an exitprot to the next instruction
               "_done_protmem_setup:\n\t"      // 3|
               :
               :"r"(&secret)
               :"t1"
               );
  printf("   ...done\n");

  /* Access the protected memory segment using the Protmem features. Note that the _protmem_func code
   * copies four bytes from the address it finds in a3 to the address it finds in a4, and then does
   * exitprot to the address it finds in a5 (this is set by the "enterprot a5" line)
   */
  printf("beginning correct protmem use...\n");
  asm volatile(
               "mv a3,%0\n\t"
               "mv a4,%1\n\t"
               "enterprot a5\n\t"
               :
               :"r"(&secret), "r"(&nonsecret)
               :"a3","a4","a5","t1","memory" // note that _protmem_func clobbers t1, but we can have gcc
               );                            // take care of that
  printf("   ...done\n");
  printf("nonsecret is %d %d %d %d\n",nonsecret[0],nonsecret[1],nonsecret[2],nonsecret[3]);

  /* Finally, we try to access the protected memory segment while in Protmem locked mode. This will
     cause a "protmem violation" error at runtime.
   */
  printf("beginning incorrect protmem use...\n");
  nonsecret[0] = secret[0];
  printf("   ...done (this means Protmem FAILED)\n"); // if Protmem works correctly, we never hit this line

}

/* This asm defines the code which will be pointed to by the Protmem instruction pointer.
   It just copies four bytes from the address it finds in a3 to the address it finds in a4,
   before exitprot-ing back to the address it finds in a5. The exitprot function puts us in
   Protmem locked mode before jumping to the address in the register passed to it.

   Note that in a real program using Protmem, we would need to have some mechanism for calling
   this code to permanently exit from Protmem lock mode when we are done with the data in the
   protected memory segment. We might use some register as a flag for this. When called in
   this way, the code should zero out the contents of the protected memory segment and return
   to the caller via a normal jump, rather than exitprot, thus preserving the unlocked state.
   However, since this demonstrator always gets killed by a Protmem violation fault, we do not
   need this here.
 */
asm(
    "_protmem_func:\n\t"
    "lb t1, 0(a3)\n\t"
    "sb t1, 0(a4)\n\t"
    "lb t1, 1(a3)\n\t"
    "sb t1, 1(a4)\n\t"
    "lb t1, 2(a3)\n\t"
    "sb t1, 2(a4)\n\t"
    "lb t1, 3(a3)\n\t"
    "sb t1, 3(a4)\n\t"
    "exitprot a5\n\t"
    );
