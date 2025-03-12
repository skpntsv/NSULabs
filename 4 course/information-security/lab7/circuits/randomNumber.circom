template LCG(a, c, m) {
    signal input seed;
    signal output random;

    signal temp;
    signal result;

    result <== a * seed + c;
    temp <== result - (result / m) * m;
    random <== temp;
}

component main = LCG(1664525, 1013904223, 4294967296);
