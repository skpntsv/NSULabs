(ns lab2.core-test
  (:require [clojure.test :refer :all]))

(defmacro is-close?
  [a b epsilon]
  `(~'< (Math/abs (~'- ~a ~b)) ~epsilon))
