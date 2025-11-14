(ns lab3.part2
  (:require [lab3.core :refer :all]
            [lab3.part1 :refer [parallel-filter]]))

(defn lazy-parallel-filter
  [pred coll macro-chunk-size]
  (lazy-seq
    (when-let [s (seq coll)]
      (let [
            macro-chunk (doall (take macro-chunk-size s))
            rest-of-coll (drop macro-chunk-size s)

            micro-chunk-size (max 1 (long (Math/ceil (/ (count macro-chunk) n-cpu))))
            processed-chunk (parallel-filter pred macro-chunk micro-chunk-size)]
        (concat processed-chunk (lazy-parallel-filter pred rest-of-coll macro-chunk-size))))))

(defn bench []
  (println "=== sqrt ===")
  (print "filter:           ")
  (time (count (filter pred-sqrt data)))

  (print "lazy-parallel-filter:  ")
  (time (count (lazy-parallel-filter pred-sqrt data chunk-size)))

  (println "=== pred-heavy ===")
  (print "filter:           ")
  (time (count (filter pred-heavy data)))

  (print "lazy-parallel-filter:  ")
  (time (count (lazy-parallel-filter pred-heavy data chunk-size)))

  (println "=== Sleep ===")
  (print "filter:           ")
  (time (count (filter pred-sleep small)))

  (print "lazy-parallel-filter:  ")
  (time (count (lazy-parallel-filter pred-sleep small chunk-size))))

(defn bench-infinite []
  (let [
        chunk-size 50
        take-n 100]

    (println "\n\nchunk-size:" chunk-size ", take-n:" take-n)

    (print "filter:                   ")
    (time (doall (take take-n (filter pred-sleep (range)))))

    (print "lazy-parallel-filter:     ")
    (time (doall (take take-n (lazy-parallel-filter pred-sleep (range) chunk-size))))

    (println "Check lazy in lazy-parallel-filter")

    (let [pfilter (lazy-parallel-filter pred-sleep (range) chunk-size)]

      (println "\n take 100")
      (time (doall (take 100 pfilter)))
      (println "\n take 200")
      (time (doall (take 200 pfilter)))
      (println "\n take 400")
      (time (doall (take 400 pfilter)))
      (println "\n take 500")
      (time (doall (take 500 pfilter)))
      (println "\n take 200")
      (time (doall (take 200 pfilter))))))

(defn -main []
  (bench)
  (bench-infinite)
  (shutdown-agents))