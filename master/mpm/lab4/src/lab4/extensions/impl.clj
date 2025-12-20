(ns lab4.extensions.impl
  (:require [lab4.ast :refer [make-not make-or]]
            [lab4.registry :refer [register-op!]]))

(defn make-imp
  "Импликация (если A, то B)."
  [a b]
  {:op :impl :args [a b]})

(register-op! :impl (fn [[a b]] (make-or (make-not a) b)))
