(ns lab3.core)

(defn chunk-seq [coll chunk-size]
  (loop [xs coll
         acc []]
    (if (empty? xs)
      acc
      (recur (drop chunk-size xs)
             (conj acc (take chunk-size xs))))))

(defn pred-sqrt [x]
  (let [y (Math/sqrt (+ x 0.00001))]
    (zero? (mod (int y) 7))))

(defn pred-sleep [x]
  (Thread/sleep 1)
  true)

(defn pred-heavy [x]
  (let [xx (double x)
        a (+ (Math/sin xx)
             (Math/cos (* xx 1.3))
             (Math/tan (* xx 0.7)))

        b (+ (Math/log1p (+ xx 10))
             (Math/sqrt (+ xx 3))
             (Math/pow xx 1.2))

        c (+ (* a b)
             (* a a)
             (* b 0.12345)
             (/ a (+ b 1)))

        d (+ (Math/sinh (/ xx 10))
             (Math/cosh (/ xx 20))
             (Math/tanh (/ xx 5)))

        res (+ (* 0.1 a)
               (* 0.2 b)
               (* 0.3 c)
               (* 0.4 d))]

    (< (Math/abs res) 1000)))

(def data (range 3000000))
(def small (range 2000))
(def chunk-size 500)

(def n-cpu (.availableProcessors (Runtime/getRuntime)))

