(ns lab4.algo.dnf
  (:require [clojure.walk :as walk]
            [lab4.algo.simplify :refer [simplify]]
            [lab4.ast :refer :all]))

(defmulti ^{:doc
            "Используется для применения законов де Моргана и снятия двойных отрицаний.
            Принимает узел `node`, к которому применяется отрицание, и возвращает
            эквивалентное выражение, где отрицание опущено на уровень ниже или устранено.

            Примеры:
              - :const -> инвертирует значение.
              - :not -> убирает отрицание (!!A -> A).
              - :and -> меняет на :or и отрицает аргументы (!(A & B) -> !A | !B).
              - :or -> меняет на :and и отрицает аргументы (!(A | B) -> !A & !B)."}

          push-negation
          type-of)


(defmethod push-negation :default [node] (make-not node))
(defmethod push-negation :const [val] (not val))
(defmethod push-negation :not [node] (first (:args node)))

(defmethod push-negation :or [node]
  (apply make-and (map push-negation (:args node))))

(defmethod push-negation :and [node]
  (apply make-or (map push-negation (:args node))))

(defn- to-nnf [expr]
  "Приводит выражение к Нормальной Форме Отрицаний (NNF).
  В NNF оператор отрицания (:not) может применяться только непосредственно
  к переменным или константам. Вложенные отрицания над операциями (:and, :or) недопустимы.

  Алгоритм:
  1. Вызывает `simplify` для раскрытия производных операций (xor, impl и т.д.).
  2. Обходит дерево и при нахождении конструкции (not (op ...)) вызывает
    `push-negation`, чтобы спустить отрицание вниз по дереву."

  (let [expanded (simplify expr)]
    (walk/postwalk
      (fn [node]
        (if (and (= :not (type-of node))
                 (map? (first (:args node)))) ;; Если внутри NOT лежит операция (не var/const)
          (push-negation (first (:args node)))
          node))
      expanded)))

(defn- distribute [args]
  "Применяет закон дистрибутивности к списку аргументов конъюнкции (:and).

   Логика:
   Преобразует выражение вида `A & (B | C)` в `(A & B) | (A & C)`.

   Алгоритм находит первый узел типа :or среди аргументов и «раскрывает скобки»,
   распределяя остальные аргументы (`others`) по каждому члену дизъюнкции.
   Если узлов :or нет, возвращает обычную конъюнкцию."
  (let [or-node (first (filter #(= :or (type-of %)) args))]
    (if-not or-node
      (apply make-and args)
      ;; Если есть OR, применяем дистрибутивность: A & (B | C) -> (A & B) | (A & C)
      (let [others (remove #(identical? or-node %) args)]
        (apply make-or
               (map (fn [or-arg]
                      (apply make-and (cons or-arg others)))
                    (:args or-node)))))))

(defn to-dnf [expr]
  "Приводит произвольное логическое выражение к Дизъюнктивной Нормальной Форме (ДНФ).

   ДНФ — это дизъюнкция конъюнкций литералов (сумма произведений),
   например: (A & B) | (A & !C) | D.

   Процесс преобразования:
   1. Приведение к NNF (`to-nnf`).
   2. Циклическое применение закона дистрибутивности (`distribute`) ко всем узлам :and.
   3. Промежуточное упрощение (`simplify`) на каждом шаге цикла."

  (let [nnf-expr (to-nnf expr)]
    (loop [current nnf-expr]
      ;; 1. Применяем дистрибутивность снизу-вверх
      (let [distributed (walk/postwalk
                          (fn [node]
                            (if (= :and (type-of node))
                              (distribute (:args node))
                              node))
                          current)
            ;; 2. Упрощаем результат (сплющиваем вложенные AND/OR, убираем дубли)
            simplified (simplify distributed)]

        ;; 3. Неподвижная точка
        (if (= current simplified)
          current
          (recur simplified))))))