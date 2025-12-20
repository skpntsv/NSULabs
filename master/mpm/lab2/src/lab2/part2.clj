(ns lab2.part2
  (:require [lab2.core :as func]))

(defn lazy-integrator [f h]
  (let [xs (iterate #(+ % h) 0.0)
        ys (map f xs)
        areas (map (fn [y1 y2] (* 0.5 h (+ y1 y2)))
                   ys (rest ys))
        partial-sums (reductions + 0.0 areas)]
    (fn [x]
      (let [n (int (/ x h))]
        (nth partial-sums n)))))


(defn -main []
  (let [h 0.05
        integrator (lazy-integrator func/square-fn h)]

    (time (let [result (integrator 9.0)]
            (println (format "-> x=9.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 2.0)]
            (println (format "-> x=2.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 1.0)]
            (println (format "-> x=1.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 12.0)]
            (println (format "-> x=12.0: %.2f" result))))
    (println "--------------------------------------------------\n")))