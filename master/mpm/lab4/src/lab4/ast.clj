(ns lab4.ast)


(defn make-var [value]
  "Создаёт переменную. Значение должно быть символом."
  {:pre [(symbol? value)]}
  value)

(defn make-const [value]
  "Создаёт булеву константу (true/false)."
  {:pre [(or (= value 0) (= value 1))]}
  (boolean value))

(defn make-not
  "Логическое отрицание."
  [x]
  {:op :not :args [x]})

(defn make-and
  "Конъюнкция (логическое И)."
  [& xs]
  {:op :and :args (vec xs)})

(defn make-or
  "Дизъюнкция (логическое ИЛИ)."
  [& xs]
  {:op :or :args (vec xs)})

(defn type-of [node]
  "Определяет тип узла.
   Возвращает :var, :const, :and, :or, :not или :custom."
  (cond
    (symbol? node) :var
    (boolean? node) :const
    (map? node) (let [op (:op node)]
                  (if (#{:and :or :not} op)
                    op
                    :custom))
    :else :unknown))

(defn negation? [node]
  "Является ли узел отрицанием."
  (= :not (type-of node)))