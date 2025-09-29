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

| Register name | Description |
|---------------|-------------|
| Protmem Instruction Pointer | Holds the address of the first instruction of the code path which may access the PEM |
| Protmem Data 1 | Holds the address of the start of the PEM |
| Protmem Data 2 | Most significant bit is a flag which controls Protmem lock/unlock, other 63 bits hold the length of the PEM |

| Instruction name and arguments | Description |
|--------------------------------|-------------|
| setproti rs1 | Sets Protmem Instruction Pointer to the value in rs1. Only legal in Protmem unlock mode, triggers a fault if used in lock mode |
| setprotd rs1,rs2 | Sets Protmem Data 1 to rs1, and the least significant 63 bits of Protmem Data 2 to the least significant 63 bits of rs2. Only legal in Protmem unlock mode, triggers a fault if used in lock mode|
| enterprot rd | Stores the address of the next instruction in rd, enters Protmem unlock mode, and jumps to the address in Protmem Instruction Pointer |
| exitprot rs1 | enters Protmem lock mode (i.e. exits unlock mode) and jumps to the address in rs1 |



## How to run this code
docker compose run gem5-protmem-demo