pragma circom 2.2.1;

include "comparators.circom";

template QuadraticVerifier() {
    signal input a;
    signal input b;      
    signal input c;       
    signal input x1;
    signal input x2;

    signal output is_valid;

    signal x1_squared;    // x1^2
    signal ax1_squared;   // a * x1^2
    signal bx1;           // b * x1
    signal computed_c1;   // ax1_squared + bx1 + c

    signal x2_squared;    // x2^2
    signal ax2_squared;   // a * x2^2
    signal bx2;           // b * x2
    signal computed_c2;   // ax2_squared + bx2 + c

    x1_squared <== x1 * x1;
    ax1_squared <== a * x1_squared;
    bx1 <== b * x1;
    computed_c1 <== ax1_squared + bx1 + c;

    x2_squared <== x2 * x2;
    ax2_squared <== a * x2_squared;
    bx2 <== b * x2;
    computed_c2 <== ax2_squared + bx2 + c;

    component is_c1_zero = IsZero();
    component is_c2_zero = IsZero();

    is_c1_zero.in <== computed_c1;
    is_c2_zero.in <== computed_c2;

    is_valid <== is_c1_zero.out * is_c2_zero.out;
}

component main = QuadraticVerifier();
