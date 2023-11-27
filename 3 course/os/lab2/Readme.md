# Задачи на синхронизацию

## [Блок задач на "удовлетворительно"](./task1)

### 2.1. Проблема конкурентного доступа к разделяемому ресурсу

- a. В каталоге `sync` репозитория [git@github.com:mrutman/os.git](git@github.com:mrutman/os.git) вы найдете простую реализацию очереди на списке. Изучите код, соберите и запустите программу `queue-example.c`. Посмотрите вывод программы и убедитесь, что он соответствует вашему пониманию работы данной реализации очереди. Добавьте реализацию функции `queue_destroy()`.

- b. Изучите код программы `queue-threads.c` и разберитесь, что она делает. Соберите программу.

    - I. Запустите программу несколько раз. Если появляются ошибки выполнения, попытайтесь их объяснить и определить, что именно вызывает ошибку. Какие именно ошибки вы наблюдали?

    - II. Поиграйте следующими параметрами:

        - Размером очереди (задается в `queue_init()`). Запустите программу с размером очереди от 1000 до 1000000.
        
        - Привязкой к процессору (задается функцией `set_cpu()`). Привяжите потоки к одному процессору (ядру) и к разным.
        - Планированием потоков (функция `sched_yield()`). Попробуйте убрать эту функцию перед созданием второго потока.
        - Объясните наблюдаемые результаты.

### 2.2. Синхронизация доступа к разделяемому ресурсу

- a. Измените реализацию очереди, добавив      спинлок для синхронизации доступа к разделяемым данным. Убедитесь, что не возникает ошибок передачи данных через очередь.

- b. Убедитесь, что не возникает ошибок передачи данных через очередь.

- c. Поиграйте параметрами из пункта 2.1:

    - Оцените загрузку процессора.

    - Оцените время проведенное в пользовательском режиме и в режиме ядра.
    - Оцените текущую заполненность очереди, количество попыток чтения-записи и количество прочитанных-записанных данных.
    - Объясните наблюдаемые результаты.


- d. Часто бывает, что поток, пишущий данные в очередь, вынужден ожидать их (например, из сети на `select()`/`poll()`). Проэмулируйте эту ситуацию, добавив в поточную функцию писателя периодический вызов `usleep(1)`. Выполните задания из пункта b.

- e. Измените реализацию очереди, заменив спинлок на мутекс. Проделайте задания из пунктов b, c и d. Сравните со спинлоком.

- f. Измените реализацию очереди, добавив условную переменную. Проделайте задания из пунктов b, c и d. Сравните со спинлоком и мутексом.

- g. Используйте для синхронизации доступа к очереди семафоры. Проделайте задания из пунктов b, c и d. Сравните со спинлоком, мутексом и условной переменной.


## [Блок задач на "хорошо"](./task2)

### 2.3 Реализуйте односвязный список, хранящий строки длиной менее 100 символов, у которого с каждым элементом связан отдельный примитив синхронизации. 

* Объявление такого списка может выглядеть, например, так:
```c
typedef struct _Node {
    char value[100];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;
```
```c
typedef struct _Storage {
Node *first;
} Storage;
```

Первый поток пробегает по всему хранилищу и ищет количество пар строк, идущих по
возрастанию длины. Как только достигнут конец списка, поток инкрементирует
глобальную переменную, в которой хранится, количество выполненных им итераций и
сразу начинает новый поиск.

Второй поток пробегает по всему хранилищу и ищет количество пар строк, идущих по
убыванию длины. Как только достигнут конец списка, поток инкрементирует
глобальную переменную, в которой хранится количество выполненных им итераций и
сразу начинает новый поиск.

Третий поток пробегает по всему хранилищу и ищет количество пар строк, имеющих
одинаковую длину. Как только достигнут конец списка, поток инкрементирует
глобальную переменную, в которой хранится количество выполненных им итераций и
сразу начинает новый поиск.

Запускает 3 потока, которые в непрерывном бесконечном цикле случайным образом
проверяют - требуется ли переставлять соседние элементы списка (не значения) и
выполняют перестановку. Каждая успешная попытка перестановки фиксируется в
соответствующей глобальной переменной-счетчике.

Используйте для синхронизации доступа к элементам списка спинлоки, мутексы и
блокировки чтения-записи. Понаблюдайте как изменяются (и изменяются ли) значения
переменных счетчиков и объясните результат. Проверьте для списков длины 100, 1000,
10000, 100000

### При реализации обратите внимание на следующие пункты:
- продумайте ваше решение, чтобы избежать ошибок соревнования.
- необходимо блокировать все записи с данными которых производится работа.
- при перестановке записей списка, необходимо блокировать три записи.
- чтобы избежать мертвых блокировок, примитивы записей, более близких к началу
списка, всегда захватывайте раньше.


## [Блок задач на "отлично"](./task3)

// TODO написать задание