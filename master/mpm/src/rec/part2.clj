(ns rec.part2)

(defn _add-single-character [string-list char acc]
  (if (seq string-list)
    (if (= char (str (first (first string-list))))
      (recur (rest string-list) char acc)
      (let [acc-delta (.concat (str char) (first string-list))
            new-acc (cons acc-delta acc)]
        (recur (rest string-list) char new-acc)))
    acc))

(defn add-single-character [string-list char]
  (_add-single-character string-list char '()))

(defn _add-multiple-characters [string-list chars acc]
  (if (seq chars)
    (let [acc-delta (add-single-character string-list (first chars))
          new-acc (concat acc acc-delta)]
      (recur string-list (rest chars) new-acc))
    acc))

(defn add-multiple-characters [string-list chars]
  (_add-multiple-characters string-list chars '()))

(defn _get-all-sequences [length chars acc]
  {:pre [(> length 0)]}
  (if (> length 1)
    (let [new-acc (add-multiple-characters acc chars)]
      (recur (dec length) chars new-acc))
    acc))

(defn get-all-sequences [length chars]
  (_get-all-sequences length chars chars))

(defn -main [& _]
  (let [n 2
        cs '("a" "b" "c")
        result (get-all-sequences n cs)]
    (println "Result:" result)))