write = 0x04
exit  = 0xfc
.text
_start:
        movl    $1, %ebx
        lea     str, %ecx
        movl    $len, %edx
        movl    $write, %eax
        int     $0x80
        xorl    %ebx, %ebx
        movl    $exit, %eax
        int     $0x80
.data
str:    .ascii "Hello, world!\n"
len = . -str
.globl  _start
