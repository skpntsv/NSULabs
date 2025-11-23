(ns lab4.extensions.nor
  (:require [lab4.ast :refer [make-not make-or]]
            [lab4.registry :refer [register-op!]]))

(defn make-nor
  "NOR (стрелка Пирса): отрицание (A OR B)."
  [a b]
  {:op :nor :args [a b]})

(register-op! :nor (fn [[a b]] (make-not (make-or a b))))
