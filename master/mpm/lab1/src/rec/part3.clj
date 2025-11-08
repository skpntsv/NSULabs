(ns rec.part3)

(defn my-map [f coll]
  (reduce
    (fn [acc el]
      (let [acc-delta (f el)]
        (concat acc (list acc-delta))))
    '()
    coll))

(defn my-filter [pred coll]
  (reduce
    (fn [acc el]
      (if (pred el)
        (concat acc (list el))
        acc))
    '()
    coll))

(defn -main [& _]
  (println (map inc [1,2,3,4]))
  (println (my-map inc [1,2,3,4]))

  (println (filter even? [1,2,3,4]))
  (println (my-filter even? [1,2,3,4])))