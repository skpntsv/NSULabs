import numpy as np
import matplotlib.pyplot as plt

# Чтение чисел из файла
filename = "data.txt"  # Замените на путь к вашему файлу
data = []

with open(filename, "r") as file:
    for line in file:
        numbers = [float(x) for x in line.strip().split()]
        data.extend(numbers)

# Удаление повторяющихся значений
data = np.unique(data)

# Добавление двух дополнительных точек
data = np.concatenate(([-np.inf, np.min(data)], data))

# Вычисление значений эмпирической функции распределения
y = np.arange(0, len(data)) / (len(data) - 1)

# Построение графика
fig, ax = plt.subplots()
ax.step(data, y, where='post', color='black', linestyle='dotted')
ax.set_xlabel('x')
ax.set_ylabel('F(x)')
ax.set_title('Эмпирическая функция распределение')

# Добавление стрелочек справа налево на горизонтальных синих линиях
for i in range(1, len(data)):
    ax.annotate('', xy=(data[i - 1], y[i - 1]), xytext=(data[i], y[i - 1]), arrowprops=dict(arrowstyle='->', color='blue'))

# Добавление стрелочки от 0 до min(data)
ax.annotate('', xy=(0, 0), xytext=(data[1], 0), arrowprops=dict(arrowstyle='->', color='blue'))

ax.grid(True)

# Установка пределов осей
ax.set_xlim(0, max(data) + 0.001)
ax.set_ylim(0, 1)

plt.show()
