pragma circom 2.0.0;

include "comparators.circom";

template AgeVerification(bits) {
    signal input age;
    signal output isAdult;

    signal threshold; 
    threshold <== 18;

    component comparator = GreaterEqThan(bits);
    comparator.in[0] <== age;
    comparator.in[1] <== threshold;

    isAdult <== comparator.out;
}

// Пример использования
component main = AgeVerification(8);
