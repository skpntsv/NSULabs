import numpy as np

def create_tridiagonal_matrix(n):
    tridiagonal_matrix = np.zeros((n, n))
    for i in range(n):
        if i > 0:
            tridiagonal_matrix[i, i - 1] = -1
        tridiagonal_matrix[i, i] = 2
        if i < n - 1:
            tridiagonal_matrix[i, i + 1] = -1
    return tridiagonal_matrix

def create_gamma_tridiagonal_matrix(n, gamma):
    gamma_matrix = np.zeros((n, n))
    for i in range(n):
        if i > 0:
            gamma_matrix[i, i - 1] = -1
        gamma_matrix[i, i] = 2 * (i + 1) + gamma
        if i < n - 1:
            gamma_matrix[i, i + 1] = -1
    return gamma_matrix

def calculate_alphas(matrix, n):
    alphas = np.zeros(n - 1)
    alphas[0] = -matrix[0, 1] / matrix[0, 0]
    for i in range(1, n - 1):
        alphas[i] = (-matrix[i, i + 1]) / (matrix[i, i] + matrix[i, i - 1] * alphas[i - 1])
    return alphas

def calculate_betas(matrix, alphas, f, n):
    betas = np.zeros(n)
    betas[0] = f[0] / matrix[0, 0]
    for i in range(1, n):
        betas[i] = (f[i] - matrix[i, i - 1] * betas[i - 1]) / (matrix[i, i] + matrix[i, i - 1] * alphas[i - 1])
    return betas

def calculate_solution(alphas, betas, n):
    solution = np.zeros(n)
    solution[n - 1] = round(betas[n - 1], 5)
    for i in range(n - 2, -1, -1):
        solution[i] = round(alphas[i] * solution[i + 1] + betas[i], 5)
    return solution

def main():
    size = int(input("Введите размер N: "))
    if size <= 1:
        print("Введите N > 1, попробуйте ещё раз")
        main()
    
    epsilon = float(input("Введите epsilon: "))
    gamma = float(input("Введите gamma: "))

    if gamma == 0:
        matrix = create_tridiagonal_matrix(size)
        f = np.full(size, 2 + epsilon)
    else:
        matrix = create_gamma_tridiagonal_matrix(size, gamma)
        f = np.array([2 * (i + 2) + gamma for i in range(size)])

    alphas = calculate_alphas(matrix, size)
    betas = calculate_betas(matrix, alphas, f, size)

    solution = calculate_solution(alphas, betas, size)
    for i, value in enumerate(solution, start=1):
        print(f'x[{i}] = {value}')

if __name__ == '__main__':
    main()
