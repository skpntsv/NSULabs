(ns lab4.eval
  (:require [clojure.walk :as walk]
            [lab4.ast :refer :all]))

(defn substitute [expr env]
  "Выполняет подстановку значений переменных в выражение (AST).

   Проходит по всему дереву выражения. Если находит узел-переменную (Symbol),
   которая присутствует в словаре окружения `env`, заменяет её на соответствующую
   логическую константу.

   Аргументы:
   - expr: Исходное выражение (AST).
   - env: Словарь (Map) вида `{символ значение}`, например `{'A true, 'B false}`."
  (walk/postwalk
    (fn [node]
      (if (and (symbol? node) (contains? env node))
        (make-const (get env node))
        node))
    expr))