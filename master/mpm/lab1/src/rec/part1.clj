(ns rec.part1)

(defn add-single-character [string-list char]
  (if (seq string-list)
    (if (= char (str (first (first string-list))))
      (add-single-character (rest string-list) char)
      (cons (.concat (str char) (first string-list))
            (add-single-character (rest string-list) char)))
    '()))

(defn add-multiple-characters [string-list chars acc]
  (if (seq chars)
    (add-multiple-characters string-list
                             (rest chars)
                             (concat acc (add-single-character string-list (first chars))))
    acc))

(defn get-all-sequences [length chars]
  {:pre [(> length 0)]}
  (if (> length 1)
    (add-multiple-characters (get-all-sequences (dec length) chars) chars '())
    chars))

(defn -main [& _]
  (let [n 2
        cs '("a" "b" "c")]
    (println "Result:" (get-all-sequences n cs))))