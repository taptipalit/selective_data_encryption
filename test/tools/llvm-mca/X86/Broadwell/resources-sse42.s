# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=broadwell -instruction-tables < %s | FileCheck %s

crc32b      %al, %ecx
crc32b      (%rax), %ecx

crc32l      %eax, %ecx
crc32l      (%rax), %ecx

crc32w      %ax, %ecx
crc32w      (%rax), %ecx

crc32b      %al, %rcx
crc32b      (%rax), %rcx

crc32q      %rax, %rcx
crc32q      (%rax), %rcx

pcmpestri   $1, %xmm0, %xmm2
pcmpestri   $1, (%rax), %xmm2

pcmpestrm   $1, %xmm0, %xmm2
pcmpestrm   $1, (%rax), %xmm2

pcmpistri   $1, %xmm0, %xmm2
pcmpistri   $1, (%rax), %xmm2

pcmpistrm   $1, %xmm0, %xmm2
pcmpistrm   $1, (%rax), %xmm2

pcmpgtq     %xmm0, %xmm2
pcmpgtq     (%rax), %xmm2

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      3     1.00                        crc32b	%al, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32b	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32l	%eax, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32l	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32w	%ax, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32w	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32b	%al, %rcx
# CHECK-NEXT:  2      8     1.00    *                   crc32b	(%rax), %rcx
# CHECK-NEXT:  1      3     1.00                        crc32q	%rax, %rcx
# CHECK-NEXT:  2      8     1.00    *                   crc32q	(%rax), %rcx
# CHECK-NEXT:  8      18    4.00                        pcmpestri	$1, %xmm0, %xmm2
# CHECK-NEXT:  9      23    4.00    *                   pcmpestri	$1, (%rax), %xmm2
# CHECK-NEXT:  9      19    4.00                        pcmpestrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  10     24    4.00    *                   pcmpestrm	$1, (%rax), %xmm2
# CHECK-NEXT:  3      11    3.00                        pcmpistri	$1, %xmm0, %xmm2
# CHECK-NEXT:  4      16    3.00    *                   pcmpistri	$1, (%rax), %xmm2
# CHECK-NEXT:  3      11    3.00                        pcmpistrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  4      16    3.00    *                   pcmpistrm	$1, (%rax), %xmm2
# CHECK-NEXT:  1      5     1.00                        pcmpgtq	%xmm0, %xmm2
# CHECK-NEXT:  2      10    1.00    *                   pcmpgtq	(%rax), %xmm2

# CHECK:      Resources:
# CHECK-NEXT: [0]   - BWDivider
# CHECK-NEXT: [1]   - BWFPDivider
# CHECK-NEXT: [2]   - BWPort0
# CHECK-NEXT: [3]   - BWPort1
# CHECK-NEXT: [4]   - BWPort2
# CHECK-NEXT: [5]   - BWPort3
# CHECK-NEXT: [6]   - BWPort4
# CHECK-NEXT: [7]   - BWPort5
# CHECK-NEXT: [8]   - BWPort6
# CHECK-NEXT: [9]   - BWPort7

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]
# CHECK-NEXT:  -      -     31.67  11.67  5.00   5.00    -     13.67  1.00    -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    Instructions:
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -      -      -     crc32b	%al, %ecx
# CHECK-NEXT:  -      -      -     1.00   0.50   0.50    -      -      -      -     crc32b	(%rax), %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -      -      -     crc32l	%eax, %ecx
# CHECK-NEXT:  -      -      -     1.00   0.50   0.50    -      -      -      -     crc32l	(%rax), %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -      -      -     crc32w	%ax, %ecx
# CHECK-NEXT:  -      -      -     1.00   0.50   0.50    -      -      -      -     crc32w	(%rax), %ecx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -      -      -     crc32b	%al, %rcx
# CHECK-NEXT:  -      -      -     1.00   0.50   0.50    -      -      -      -     crc32b	(%rax), %rcx
# CHECK-NEXT:  -      -      -     1.00    -      -      -      -      -      -     crc32q	%rax, %rcx
# CHECK-NEXT:  -      -      -     1.00   0.50   0.50    -      -      -      -     crc32q	(%rax), %rcx
# CHECK-NEXT:  -      -     4.25   0.25    -      -      -     3.25   0.25    -     pcmpestri	$1, %xmm0, %xmm2
# CHECK-NEXT:  -      -     4.25   0.25   0.50   0.50    -     3.25   0.25    -     pcmpestri	$1, (%rax), %xmm2
# CHECK-NEXT:  -      -     4.58   0.58    -      -      -     3.58   0.25    -     pcmpestrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  -      -     4.58   0.58   0.50   0.50    -     3.58   0.25    -     pcmpestrm	$1, (%rax), %xmm2
# CHECK-NEXT:  -      -     3.00    -      -      -      -      -      -      -     pcmpistri	$1, %xmm0, %xmm2
# CHECK-NEXT:  -      -     3.00    -     0.50   0.50    -      -      -      -     pcmpistri	$1, (%rax), %xmm2
# CHECK-NEXT:  -      -     3.00    -      -      -      -      -      -      -     pcmpistrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  -      -     3.00    -     0.50   0.50    -      -      -      -     pcmpistrm	$1, (%rax), %xmm2
# CHECK-NEXT:  -      -     1.00    -      -      -      -      -      -      -     pcmpgtq	%xmm0, %xmm2
# CHECK-NEXT:  -      -     1.00    -     0.50   0.50    -      -      -      -     pcmpgtq	(%rax), %xmm2

