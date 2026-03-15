    .section .text.start
    .globl _start
    .extern main
    .extern goodtrap
    .extern badtrap

_start:
    # 初始化栈指针
    lu12i.w    $sp, 0x0000F       # sp = 0x0000F000

    # 调用 main()
    bl         main

    # main 返回值在 a0 ($r4)
    # 若返回 0，则 goodtrap；否则 badtrap(a0)

    addi.w     $t0, $zero, 0
    beq        $a0, $t0, pass_path

fail_path:
    # badtrap(a0)
    jirl       $ra, badtrap, 0

pass_path:
    # goodtrap()
    jirl       $ra, goodtrap, 0

hang:
    b          hang