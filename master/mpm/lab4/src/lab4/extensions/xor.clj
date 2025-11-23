(ns lab4.extensions.xor
  (:require [lab4.ast :refer [make-and make-not make-or]]
            [lab4.registry :refer [register-op!]]))

(defn make-xor
  "Исключающее ИЛИ."
  [a b]
  {:op :xor :args [a b]})

(register-op! :xor (fn [[a b]] (make-or (make-and a (make-not b)) (make-and (make-not a) b))))