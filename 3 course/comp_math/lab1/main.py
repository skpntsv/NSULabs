# -*- coding: utf-8 -*-

import math

a, b, c, epsilon = map(float, input("Введите значения a, b, c и epsilon через пробел: ").split())
delta = 1000
iterations = 0

def f(x):
    return x**3 + a * x**2 + b * x + c

def find_extremes():
    discriminant = a**2 - 3 * b
    y0 = -discriminant / 3

    if y0 >= epsilon:
        return None, None

    extreme1 = (-a - math.sqrt(discriminant)) / 3
    extreme2 = (-a + math.sqrt(discriminant)) / 3

    return extreme1, extreme2

def find_border(border1, toRight):
    value1 = f(border1)
    step = delta if toRight else -delta
    border2 = border1 + step

    while value1 * f(border2) > 0:
        border2 += step

    return border2

def dichotomy_method(border1, border2):
    global iterations

    f1 = f(border1)
    f2 = f(border2)

    if abs(f1) < epsilon:
        return border1
    if abs(f2) < epsilon:
        return border2

    while True:
        iterations += 1

        c = (border1 + border2) / 2
        f_midlle = f(c)

        if abs(f_midlle) < epsilon:
            return c
        if f1 * f_midlle < 0:
            border2 = c
        else:
            border1 = c
            f1 = f_midlle

alpha, betta = find_extremes()
roots = []

if alpha is None:
    border1 = 0
    f_0 = f(border1)
    if abs(f_0) < epsilon:
        roots.append((border1, 3))
    elif f_0 < -epsilon:
        border2 = find_border(0, True)
        roots.append((dichotomy_method(border1, border2), 1))
    else:
        border2 = find_border(0, False)
        roots.append((dichotomy_method(border1, border2), 1))
else:
    f1 = f(alpha)
    f2 = f(betta)

    if f1 >= epsilon and f2 >= epsilon:
        border1 = find_border(alpha, False)
        roots.append((dichotomy_method(border1, alpha), 1))
    elif f1 <= -epsilon and f2 <= -epsilon:
        border2 = find_border(betta, True)
        roots.append((dichotomy_method(alpha, border2), 1))
    elif abs(f1) < epsilon and abs(f2) < epsilon:
        roots.append((alpha, 3))  
    elif f1 >= epsilon and abs(f2) < epsilon:
        border1 = find_border(alpha, False)
        roots.append((dichotomy_method(border1, alpha), 1))  
        roots.append((betta, 2))  
    elif abs(f1) < epsilon and f2 <= -epsilon:
        border2 = find_border(betta, True)
        roots.append((alpha, 2)) 
        roots.append((dichotomy_method(betta, border2), 1))
    elif f1 >= epsilon and f2 <= -epsilon:
        border1 = find_border(alpha, False)
        border2 = find_border(betta, True)
        roots.append((dichotomy_method(border1, alpha), 1))
        roots.append((dichotomy_method(alpha, betta), 1))
        roots.append((dichotomy_method(betta, border2), 1))

if roots:
    print("-----------------------------------")
    for root, multiplicity in roots:
        print(f"Корень: {root}, Кратность: {multiplicity}")
    print(f"\nКоличество итераций равно {iterations}")
    print("-----------------------------------")
else:
    print("Нет корней")
