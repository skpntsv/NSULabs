(ns lab4.core-test
  (:require [clojure.set :as set]
            [clojure.test :refer :all]
            [lab4.core :refer :all]))

(defn vars-of
  "Рекурсивно извлекает все символы переменных из AST."
  [expr]
  (cond
    (symbol? expr) #{expr}
    (map? expr) (apply set/union (map vars-of (:args expr)))
    :else #{}))

(defn eval-expr
  "Интерпретатор булевых выражений.
   Используется для построения таблиц истинности."
  [expr env]
  (cond
    (boolean? expr) expr
    ;; Если переменной нет в env, считаем её false
    (symbol? expr) (get env expr false)
    (map? expr)
    (let [op (:op expr)
          args (:args expr)
          ;; Рекурсивно вычисляем аргументы
          vals (map #(eval-expr % env) args)]
      (case op
        :not (not (first vals))
        :and (boolean (every? true? vals))
        :or (boolean (some true? vals))

        :xor (let [[a b] vals] (not= a b))
        :impl (let [[a b] vals] (or (not a) b))
        :nor (not (some true? vals))
        :nand (not (every? true? vals))
        (throw (ex-info "Неизвестная операция в eval" {:op op}))))
    :else (throw (ex-info "Некорректное выражение" {:expr expr}))))

(defn all-assignments
  "Генерирует все возможные комбинации true/false для заданных переменных.
   Пример для {A, B}: ({A false, B false} {A false, B true} ...)"
  [vars]
  (if (empty? vars)
    [{}]
    (let [[v & vs] (seq vars)
          tail-envs (all-assignments (set vs))]
      (concat
        (for [m tail-envs] (assoc m v false))
        (for [m tail-envs] (assoc m v true))))))

(defn equivalent?
  "Проверяет семантическую эквивалентность двух формул.
   Строит полную таблицу истинности для обоих выражений и сравнивает результаты."
  [e1 e2]
  (let [vars (set/union (vars-of e1) (vars-of e2))]
    (every?
      (fn [env]
        (= (eval-expr e1 env)
           (eval-expr e2 env)))
      (all-assignments vars))))

;; =============================================================================
;; Базовые элементы
;; =============================================================================

(deftest ast-structure-test
  (testing "Проверка того, что конструкторы создают ожидаемую структуру данных (Map)"
    (is (= 'A (make-var 'A)))
    (is (= true (make-const 1)))
    (is (= {:op :not :args ['A]} (make-not 'A)))
    (is (= {:op :and :args ['A 'B]} (make-and 'A 'B)))
    (is (= {:op :or :args ['A 'B]} (make-or 'A 'B)))
    ;; Производные операции тоже должны создавать узлы AST
    (is (= {:op :impl :args ['A 'B]} (make-imp 'A 'B)))
    (is (= {:op :xor :args ['A 'B]} (make-xor 'A 'B)))))

;; =============================================================================
;; Алгебраические упрощения
;; =============================================================================

(deftest simplification-rules-test
  (testing "Удаление нейтральных элементов"
    ;; A & true -> A
    (is (= 'A (simplify (make-and 'A true))))
    ;; A & false -> false
    (is (= false (simplify (make-and 'A false))))
    ;; A | false -> A
    (is (= 'A (simplify (make-or 'A false))))
    ;; A | true -> true
    (is (= true (simplify (make-or 'A true)))))

  (testing "Снятие двойного отрицания !!A -> A"
    (let [expr (make-not (make-not 'B))]
      (is (= 'B (simplify expr)))))

  (testing "Раскрытие вложенных ассоциативных операций"
    ;; (A & (B & C)) -> (A & B & C)
    (let [nested (make-and 'A (make-and 'B 'C))
          flat (simplify nested)]
      (is (= :and (:op flat)))
      (is (= 3 (count (:args flat)))))))

(deftest desugar-check-test
  (testing "Функция simplify должна раскрывать (imp, xor...)"
    ;; Проверяем, что после упрощения в дереве НЕ остается операций :imp или :xor
    (let [complex-expr (make-and (make-imp 'A 'B) (make-xor 'C 'D))
          simplified (simplify complex-expr)]
      ;; Обходим дерево результата
      (let [found-sugar (atom false)]
        (clojure.walk/postwalk
          (fn [node]
            (when (and (map? node) (#{:imp :xor} (:op node)))
              (reset! found-sugar true))
            node)
          simplified)
        (is (false? @found-sugar) "В упрощенном выражении не должно быть :imp или :xor")))))

;; =============================================================================
;; Алгоритм ДНФ
;; =============================================================================

(deftest dnf-structure-cases-test
  (testing "Проверка конкретных сценариев приведения к ДНФ"
    (let [scenarios
          [;; 1. Уже готовая ДНФ: A | (B & C)
           ;; Алгоритм не должен ломать то, что уже работает.
           {:doc      "Identity: Expression is already in DNF"
            :in       (make-or 'A (make-and 'B 'C))
            :expected (make-or 'A (make-and 'B 'C))}

           ;; 2. Дистрибутивность справа: (A | B) & C -> (A & C) | (B & C)
           {:doc      "Right Distributivity: (A or B) and C"
            :in       (make-and (make-or 'A 'B) 'C)
            :expected (make-or (make-and 'A 'C)
                               (make-and 'B 'C))}

           ;; 3. Импликация: A -> B превращается в !A | B
           {:doc      "Implication expansion"
            :in       (make-imp 'A 'B)
            :expected (make-or 'B (make-not 'A))}

           ;; 4. XOR: A ^ B -> (A & !B) | (!A & B)
           {:doc      "XOR expansion"
            :in       (make-xor 'A 'B)
            :expected (make-or (make-and 'A (make-not 'B))
                               (make-and 'B (make-not 'A)))}

           ;; 5. Закон де Моргана для вложенности: !(A | B) -> !A & !B
           {:doc      "De Morgan: not (A or B)"
            :in       (make-not (make-or 'A 'B))
            :expected (make-and (make-not 'A) (make-not 'B))}

           ;; 6. Сложный случай: (A -> B) & C
           ;; (!A | B) & C -> (!A & C) | (B & C)
           {:doc      "Complex: (A imp B) and C"
            :in       (make-and (make-imp 'A 'B) 'C)
            :expected (make-or (make-and 'B 'C)
                               (make-and 'C (make-not 'A)))}]]

      (doseq [{:keys [doc in expected]} scenarios]
        (let [result (to-dnf in)]
          ;; Проверка 1: Логическая эквивалентность (самое важное)
          (is (equivalent? in result)
              (str "[" doc "] Результат не эквивалентен исходному выражению!"))

          ;; Проверка 2: Структурное совпадение
          (is (= expected result)
              (str "[" doc "] Структура ДНФ не совпадает с ожидаемой.\n"
                   "Ожидалось: " expected "\n"
                   "Получено:  " result)))))))

;; =============================================================================
;; Подстановка
;; =============================================================================

(deftest substitution-logic-test
  (testing "Подстановка переменных с последующим приведением к ДНФ"
    ;; Пример: XOR(A, B), где A = false.
    ;; 0 ^ B = B. Ожидаем просто символ B.
    (let [expr (make-xor 'A 'B)
          res (substitute-and-dnf expr {'A false})]
      (is (= 'B res)))

    ;; Пример: (A -> B), где B = false.
    ;; A -> 0 === !A | 0 === !A.
    (let [expr (make-imp 'A 'B)
          res (substitute-and-dnf expr {'B false})]
      (is (= (make-not 'A) res)))

    ;; Пример: Полное вычисление. (A & B) | C, все true.
    (let [expr (make-or (make-and 'A 'B) 'C)
          res (substitute-and-dnf expr {'A true 'B true 'C true})]
      (is (= true res)))))

;; =============================================================================
;; Случайные проверки
;; =============================================================================

(def test-expressions
  "Список разнообразных выражений для массовой проверки."
  [(make-imp (make-not 'A) 'B)                              ;; !A -> B
   (make-xor (make-and 'A 'B) 'C)                           ;; (A & B) ^ C
   (make-nand 'A (make-or 'B 'C))                           ;; A nand (B | C)
   (make-and (make-imp 'A 'B) (make-imp 'B 'A))])           ;; Эквиваленция A <-> B


(deftest random-equivalence-test
  (testing "Любое выражение должно быть эквивалентно своей ДНФ форме"
    (doseq [expr test-expressions]
      (let [dnf-form (to-dnf expr)]
        (is (equivalent? expr dnf-form) "Логика выражения изменилась после преобразования!")))))