(ns lab3.part1
  (:require [lab3.core :refer :all]))

(defn parallel-filter [pred coll chunk-size]
  (let [chunks (chunk-seq coll chunk-size)
        futures (map (fn [ch]
                       (future
                         (doall (filter pred ch))))
                     chunks)]
    (doall (mapcat deref futures))))

(defn -main []
  (println "=== sqrt ===")
  (print "filter:           ")
  (time (count (filter pred-sqrt data)))

  (print "parallel-filter:  ")
  (time (count (parallel-filter pred-sqrt data chunk-size)))

  (println "=== pred-heavy ===")
  (print "filter:           ")
  (time (count (filter pred-heavy data)))

  (print "parallel-filter:  ")
  (time (count (parallel-filter pred-heavy data chunk-size)))

  (println "=== Sleep ===")
  (print "filter:           ")
  (time (count (filter pred-sleep small)))

  (print "parallel-filter:  ")
  (time (count (parallel-filter pred-sleep small chunk-size))))