(ns lab2.part1_test
  (:require [clojure.test :refer :all]
            [lab2.core :as func]
            [lab2.part1 :refer [memoize-integrator]]
            [lab2.core-test :refer [is-close?]]))

(deftest memoize-integrator-test
  (let [h 0.001]
    (testing "Integral of f(x) = x"
      (let [integrator (memoize-integrator func/linear-fn h)]
        (is (is-close? (integrator 1.0) 0.5 1e-3))
        (is (is-close? (integrator 2.0) 2.0 1e-3))))

    (testing "Integral of f(x) = x^2"
      (let [integrator (memoize-integrator func/square-fn h)]
        (is (is-close? (integrator 3.0) 9.0 1e-3))))

    (testing "Integral of sin(x)"
      (let [integrator (memoize-integrator func/sin-fn h)]
        (is (is-close? (integrator Math/PI) 2.0 1e-3))))

    (testing "Integral of = e^x"
      (let [integrator (memoize-integrator func/exp-fn h)]
        (is (is-close? (integrator 2.0) (- (Math/exp 2.0) 1.0) 1e-3))))

    (testing "Corner cases"
      (let [integrator (memoize-integrator func/square-fn h)]
        (testing "Integration to zero"
          (is (= 0.0 (integrator 0.0))))

        (testing "Integration over a distance smaller than a step"
          (is (= 0.0 (integrator (* h 0.5)))))))))