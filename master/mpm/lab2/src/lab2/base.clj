(ns lab2.base
  (:require [lab2.core :as func]))

(defn basic-integrator [f h]
  (fn [x]
    (let [n (int (/ x h))
          xs (map #(* % h) (range (inc n)))
          ys (map f xs)]
      (* h 0.5
         (+ (first ys)
            (* 2 (reduce + (butlast (rest ys))))
            (last ys))))))

(defn -main []
  (let [h 0.001
        integrator (basic-integrator func/square-fn h)]

    (time (let [result (integrator 5.0)]
            (println (format "-> x=5.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 2.0)]
            (println (format "-> x=2.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 5.0)]
            (println (format "-> x=5.0: %.2f" result))))
    (println "--------------------------------------------------\n")

    (time (let [result (integrator 5.1)]
            (println (format "-> x=5.1: %.2f" result))))
    (println "--------------------------------------------------\n")))