import numpy as np

def create_matrix(n):
    matrix = np.zeros((n, n))
    for i in range(n):
        if i > 0:
            matrix[i, i - 1] = -1
        matrix[i, i] = 2
        if i < n - 1:
            matrix[i, i + 1] = -1
    return matrix

def create_gamma_matrix(n, gamma):
    matrix = np.zeros((n, n))
    for i in range(n):
        if i > 0:
            matrix[i, i - 1] = -1
        matrix[i, i] = 2 * (i + 1) + gamma
        if i < n - 1:
            matrix[i, i + 1] = -1
    return matrix

def count_alphas(matrix, n):
    alphas = np.zeros(n - 1)
    alphas[0] = -matrix[0, 1] / matrix[0, 0]
    for i in range(1, n - 1):
        alphas[i] = (-matrix[i, i + 1]) / (matrix[i, i] + matrix[i, i - 1] * alphas[i - 1])
    return alphas

def count_betas(matrix, alphas, f, n):
    betas = np.zeros(n)
    betas[0] = f[0] / matrix[0, 0]
    for i in range(1, n):
        betas[i] = (f[i] - matrix[i, i - 1] * betas[i - 1]) / (matrix[i, i] + matrix[i, i - 1] * alphas[i - 1])
    return betas

def count_x(alphas, betas, n):
    x = np.zeros(n)
    x[n - 1] = round(betas[n - 1], 5)
    for i in range(n - 2, -1, -1):
        x[i] = round(alphas[i] * x[i + 1] + betas[i], 5)
    return x

def main():
    n = int(input("Введите n > 1: "))
    assert n > 1
    eps = float(input("Введите eps: "))
    gamma = float(input("Введите gamma: "))

    if gamma == 0:
        matrix = create_matrix(n)
        f = np.full(n, 2 + eps)
    else:
        matrix = create_gamma_matrix(n, gamma)
        f = np.array([2 * (i + 2) + gamma for i in range(n)])

    alphas = count_alphas(matrix, n)
    betas = count_betas(matrix, alphas, f, n)

    x = count_x(alphas, betas, n)
    for i, value in enumerate(x, start=1):
        print(f'x[{i}] = {value}')

if __name__ == '__main__':
    main()
