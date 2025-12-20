(ns lab2.part1
  (:require [lab2.base :refer [basic-integrator]]
            [lab2.core :as func]))

(defn memoize-integrator [f h]
  (memoize (basic-integrator f h)))

(defn -main []
  (let [h 0.001
        integrator (memoize-integrator func/square-fn h)]

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