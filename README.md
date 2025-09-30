# Protmem, a simple RISCV extension
This repository contains code for a simple extension to the RISCV architecture called Protmem,
implemented as a modification to the gem5 simulator and the GNU RISCV toolchain. I have created Protmem
as a demonstration of my ability to work on gem5, rather than as a serious suggestion for a RISCV extension.

## Overview of Protmem
The essential idea of Protmem is to augment RISCV to allow a process to create a **protected memory segment**
(PMS) in its address space, which may only be accessed via one designated code path. Any other access to the
PMS will cause a processor fault. The PMS could be used, for example, to hold a cryptographic key used by a
program, with the designated code path allowed to access the PMS being an encryption/decryption routine. Protmem
would then prevent any vulnerability (such as a buffer overread) in the program's code outside the designated
code path from exposing the cryptographic key to an attacker (this could prevent vulnerabilities like
[Heartbleed](https://www.heartbleed.com/) from being exploited).

## Design of Protmem
Protmem augments 64-bit RISCV by adding 3 new 64-bit registers and four new instructions. The registers are as follows
(see below for explanation of Protmem lock/unlock mode).

| Register name               | Description |
|-----------------------------|-------------|
| Protmem Instruction Pointer | Holds the address of the first instruction of the code path which may access the PMS |
| Protmem Data 1              | Holds the address of the start of the PMS |
| Protmem Data 2              | Most significant bit is a flag which controls Protmem lock/unlock, other 63 bits hold the length of the PMS |

| Instruction name and arguments | Description |
|--------------------------------|-------------|
| `setproti rs1`                 | Sets Protmem Instruction Pointer to the value in rs1. Only legal in Protmem unlock mode, triggers a fault if used in lock mode |
| `setprotd rs1, rs2`            | Sets Protmem Data 1 to rs1, and the least significant 63 bits of Protmem Data 2 to the least significant 63 bits of rs2. Only legal in Protmem unlock mode, triggers a fault if used in lock mode|
| `enterprot rd`                 | Stores the address of the next instruction in rd, enters Protmem unlock mode, and jumps to the address in Protmem Instruction Pointer |
| `exitprot rs1`                 | Enters Protmem lock mode (i.e. exits unlock mode) and jumps to the address in rs1 |

At any point in time, the processor is in one of two states: Protmem lock, or Protmem unlock (*lock* and *unlock* for short). Which state
the processor is in is controlled by the most significant bit of the Protmem Data 2 register, and this state can only be changed via the
`enterprot` and `exitprot` commands (the bit is 1 for unlock mode and 0 for lock mode). When in unlock mode, the protected memory segment
(PMS) can be read from and written to via the load and store instructions from the standard RISCV instruction set, and the `setproti` and
`setprotd` instructions can be used to change the values in the three Protmem registers. In lock mode, none of these things are allowed, and
attempting to do them will cause a processor fault.

The processor is initialised in unlock mode. A program can then set up the PMS and the code for the Protmem Instruction Pointer to point to,
and then use `exitprot` to enter lock mode before commencing its main work.

Note that the three Protmem registers are not directly accessible via the instruction set architecture. They can only be used and modified
implicitly via the above instructions.

Protmem does not include any mechanism to protect the code pointed to by the Protmem Instruction Pointer from modification, which represents
a potential hole in Protmem's security. However, this register will normally point into the text segment of the binary, and there are already
mechanisms to protect text segments from being modified. In particular, many environments make text segments read-only. It would of course
be possible for Protmem to directly protect a section of memory beginning at the address in the Protmem Instruction Pointer, but I have not
done this.

## Using Protmem
The intended way to use Protmem is as follows. A program starts up in unlock mode. It then sets up a protected memory segment (either on the
heap or on the stack) to hold whatever data it needs to protect. It also designates the code which will be allowed to access the PMS by setting
the Protmem Instruction Pointer. After doing this, the program enters lock mode, perhaps by just doing an `exitprot` to the next instruction.

While the program is running in lock mode, it can use the `enterprot` instruction to jump to the designated code path and enter unlock mode
in order to do something with the data in the PMS. The designated code path will normally need to be supplied with some arguments via
registers or memory, and the author of the program is free to set up whatever calling convention they want to govern this.

To return to a possible use-case mentioned above, suppose we are creating some kind of program which needs to do encrypted communication.
This encryption uses a key which must be stored in memory. During the program's initial setup, we could create a PMS and copy the key into
it, before zeroing out any other copies of the key in memory. The program would also contain a function to act as the designated code path,
which would implement the encryption and decryption of data using the key. We might establish a calling convention for this function with
designated registers to hold the address of the source data, the address to write the output data to, the number of bytes to process,
the return address, and a value to designate which operation to perform. The program would then enter lock mode, and proceed to do its work.
Whenever the program has data to encrypt or decrypt, it would set up the registers according to the convention, and then jump to the
encryption/decryption routine using `enterprot`. The routine would perform the requested operation before returning using `exitprot`.

For an example of a simple usage of Protmem, see the file `workspace/demo-program/protmem-demo.c`.

## How to run this code
The files in this repository contain patches for gem5 itself as well as for the GNU RISCV toolchain from
[this repository](https://github.com/riscv-collab/riscv-gnu-toolchain). These patches add support for Protmem to gem5 and the toolchain, allowing
us to easily create binaries which use Protmem, and then run them on the modified gem5 in syscall emulation mode. There is also a demonstration
program in C with inline assembly that shows how to use the Protmem instructions.

For the user's convenience, this repository also contains files which allows everything to be easily built and run in a Docker container.
The Dockerfile sets up the container and installs all the necessary packages. The container mounts the `workspace` directory as a volume,
and all building is done in this directory, allowing easy access to the patched code. Once inside the container, running the scripts
provided will download, patch, and build both gem5 and the GNU RISCV toolchain, and then compile the Protmem demonstration program and
run the resulting binary in gem5.

To build and run everything, you can do the following. First, build and enter the container by running
```docker compose run gem5-protmem-demo```
from the root of this repository. Then, once you are at the prompt inside this container, `cd` into the `workspace` directory
and run `./full-demo.sh`. This will run each of the scripts in the `component scripts` subdirectory in order: first the toolchain will be
built, then gem5, and finally the demonstration program `workspace/demo-program/protmem-demo.c` will be compiled and run in gem5.

HERE