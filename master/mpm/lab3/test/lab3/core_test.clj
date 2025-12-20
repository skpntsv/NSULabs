(ns lab3.core-test
  (:require [clojure.test :refer :all]
            ;[lab3.core :refer :all]
            [lab3.part1 :refer [parallel-filter]]
            [lab3.part2 :refer [lazy-parallel-filter]]))

(def data (range 100))
(def chunk-size 10)

(deftest test-parallel-filter
  (is (= (filter even? data)
         (parallel-filter even? data chunk-size))))

(deftest test-lazy-parallel-filter
  (is (= (filter even? data)
         (lazy-parallel-filter even? data chunk-size))))

(deftest test-lazy-parallel-filter-finite
  (is (= (take 50 (filter odd? (range 200)))
         (take 50 (lazy-parallel-filter odd? (range 200) 7)))))

(deftest test-infinite-stream
  (let [res (take 5 (lazy-parallel-filter #(= 0 (mod % 3)) (range) 11))]
    (is (= res [0 3 6 9 12]))))