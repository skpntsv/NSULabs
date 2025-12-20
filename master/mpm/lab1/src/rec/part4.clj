(ns rec.part4)

(defn add-single-character [string-list char]
  (letfn [(pred [el] (not (= char (str (first el)))))]
    (let [tmp (filter pred string-list)]
      (map (fn [el] (.concat char el)) tmp))))

(defn add-multiple-characters [string-list chars]
  (letfn [(f [el] (add-single-character string-list el))]
    (let [tmp (map f chars)]
      (reduce concat tmp))))

(defn get-all-sequences [length chars]
  {:pre [(> length 0)]}
  (loop [current-length length
         sequences      chars]
    (if (> current-length 1)
      (recur (dec current-length)
             (add-multiple-characters sequences chars))
      sequences)))

(defn -main [& _]
  (let [n 4
        cs '("a" "b" "c")
        result (get-all-sequences n cs)]
    (println "Result:" result)))