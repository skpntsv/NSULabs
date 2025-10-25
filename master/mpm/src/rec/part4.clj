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
  (let [r (range length)
        tmp-coll (map (fn [_] chars) r)]
    (reduce add-multiple-characters tmp-coll)))

(defn -main [& _]
  (let [n 4
        cs '("a" "b" "c")
        result (get-all-sequences n cs)]
    (println "Result:" result)))