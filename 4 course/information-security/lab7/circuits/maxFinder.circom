pragma circom 2.0.0;

include "comparators.circom";

template MaxInArray(n, bits) {
    signal input arr[n];
    signal output max;

    // Промежуточные значения
    signal intermediate[n - 1];
    signal temp1[n - 1]; // Вспомогательные сигналы для хранения результатов умножения
    signal temp2[n - 1];

    signal selector[n - 1];

    component cmp[n - 1];

    for (var i = 0; i < n - 1; i++) {
        cmp[i] = GreaterThan(bits);
    }

    cmp[0].in[0] <== arr[0];
    cmp[0].in[1] <== arr[1];
    selector[0] <== cmp[0].out;

    temp1[0] <== arr[0] * selector[0];
    temp2[0] <== arr[1] * (1 - selector[0]);
    intermediate[0] <== temp1[0] + temp2[0];

    for (var i = 1; i < n - 1; i++) {
        cmp[i].in[0] <== intermediate[i - 1];
        cmp[i].in[1] <== arr[i + 1];
        selector[i] <== cmp[i].out;

        temp1[i] <== intermediate[i - 1] * selector[i];
        temp2[i] <== arr[i + 1] * (1 - selector[i]);
        intermediate[i] <== temp1[i] + temp2[i];
    }

    max <== intermediate[n - 2];
}

component main = MaxInArray(5, 16); // каждое число представлено
