(ns lab4.algo.simplify
  (:require [clojure.set :as set]
            [clojure.walk :as walk]
            [lab4.ast :refer :all]
            [lab4.registry :refer [custom-ops]]))

(defmulti ^{:doc
            "Выбирает стратегию оптимизации в зависимости от типа узла (`type-of`).

            Применяет алгебраические законы:
              - Вычисление констант (Constant folding).
              - Устранение двойного отрицания.
              - Ассоциативность (сплющивание вложенных однотипных операций).
              - Коммутативность (сортировка аргументов).
              - Идемпотентность (удаление дубликатов).
              - Законы дополнения (A & !A = 0, A | !A = 1).
              - Раскрытие производных операций (desugaring)."}


          optimize-node
          type-of)

(defmethod optimize-node :default [node] node)

;; --- Упрощение NOT ---
(defmethod optimize-node :not [{args :args}]
  (let [child (first args)]
    (cond
      (boolean? child) (not child)                          ;; !true -> false
      (= :not (type-of child)) (first (:args child))        ;; !!A -> A
      :else (make-not child))))

;; --- Упрощение AND ---
(defmethod optimize-node :and [{args :args}]
  (let [flat (distinct (mapcat #(if (= :and (type-of %)) (:args %) [%]) args))
        clean (remove true? flat)]

    ;; Проверка на противоречия (A & !A)
    (let [vars-pos (set (filter symbol? clean))
          vars-neg (set (map first (map :args (filter negation? clean))))]
      (if (not (empty? (set/intersection vars-pos vars-neg)))
        false ;; Нашли A и !A -> результат false

        ;; Обычные упрощения
        (cond
          (some false? clean) false
          (empty? clean) true
          (= 1 (count clean)) (first clean)
          :else (apply make-and (sort-by str clean)))))))

;; --- Упрощение OR ---
(defmethod optimize-node :or [{args :args}]
  (let [flat (distinct (mapcat #(if (= :or (type-of %)) (:args %) [%]) args))
        clean (remove false? flat)]

    ;; Проверка на противоречия (A & !A)
    (let [vars-pos (set (filter symbol? clean))
          vars-neg (set (map first (map :args (filter negation? clean))))]
      (if (not (empty? (set/intersection vars-pos vars-neg)))
        true ;; Нашли A и !A внутри OR -> результат true

        ;; Обычные упрощения
        (cond
          (some true? clean) true
          (empty? clean) false
          (= 1 (count clean)) (first clean)
          :else (apply make-or (sort-by str clean)))))))

;; --- Пользовательские операции ---
(defmethod optimize-node :custom [node]
  (let [op (:op node)]
    (if-let [handler (@custom-ops op)]
      (handler (:args node))
      (throw (ex-info "Unknown operation" {:op op})))))

(defn simplify [expr]
  "Выполняет полное алгебраическое упрощение выражения.

   Алгоритм:
   1. Обходит дерево снизу-вверх (postwalk), применяя `optimize-node` к каждому узлу.
   2. Повторяет процесс до достижения «неподвижной точки» (пока выражение не перестанет меняться)."
  (let [step (walk/postwalk
               (fn [node]
                 (if (map? node) (optimize-node node) node))
               expr)]
    (if (= step expr)
      step
      (recur step))))