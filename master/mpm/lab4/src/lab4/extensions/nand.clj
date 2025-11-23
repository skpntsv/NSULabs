(ns lab4.extensions.nand
  (:require [lab4.ast :refer [make-and make-not]]
            [lab4.registry :refer [register-op!]]))

(defn make-nand
  "NAND (штрих Шеффера): отрицание (A AND B)."
  [a b]
  {:op :nand :args [a b]})

(register-op! :nand (fn [[a b]] (make-not (make-and a b))))