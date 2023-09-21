from math import sqrt
import sys

def f(x, a, b, c):
    return x**3 + a * x**2 + b * x + c

def find_extremes(a, b, c):
    discriminant = find_discriminant(a, b, c)
    alpha = (-b - sqrt(discriminant)) / a
    betta = (-b + sqrt(discriminant)) / a

    if (alpha > betta):
        tmp = alpha
        alpha = betta
        betta = tmp

    return alpha, betta

def find_discriminant(a, b, c):
    discriminant = b * b - a * c

    return discriminant

def bisection_method(a, b, c, epsilon):
    # if (find_discriminant < -epsilon):
    #     count_of_roots = 1      # 1 корень

    alpha, betta = find_extremes(3, 2 * a, b)
    
    intervals = [(sys.float_info.min, alpha), (alpha, betta), (betta, 1000000)]
    roots = []

    for interval in intervals:
        a_interval, b_interval = interval
        if (f(a_interval, a, b, c) * f(b_interval, a, b, c)) < 0:
            while abs(b_interval - a_interval) < epsilon:
                if (f((b_interval - a_interval) / 2, a, b, c) * f(b_interval, a, b, c)) < 0:
                    b_interval = (b_interval - a_interval) / 2
                else:
                    a_interval = (b_interval - a_interval) / 2
            roots.append(a_interval)    

    return roots

a = float(input("Введите значение коэффициента a: "))
b = float(input("Введите значение коэффициента b: "))
c = float(input("Введите значение коэффициента c: "))

epsilon = float(input("Введите значение epsilon: "))

roots = bisection_method(a, b, c, epsilon)

if roots:
    print(f"Корни кубического уравнения: {roots}")
else:
    print("Уравнение не имеет корней.")