(ns lab4.core
  (:require [lab4.ast :as ast]
            [lab4.extensions.impl :as ext-imp]
            [lab4.extensions.nor :as ext-nor]
            [lab4.extensions.nand :as ext-nand]
            [lab4.extensions.xor :as ext-xor]
            [lab4.algo.simplify :as simp]
            [lab4.algo.dnf :as dnf]
            [lab4.eval :as ev]))

; Basic functions
(def make-and ast/make-and)
(def make-or ast/make-or)
(def make-not ast/make-not)
(def make-var ast/make-var)
(def make-const ast/make-const)

; Extensions
(def make-nor ext-nor/make-nor)
(def make-nand ext-nand/make-nand)
(def make-imp ext-imp/make-imp)
(def make-xor ext-xor/make-xor)

(defn simplify [expr]
  (simp/simplify expr))

(defn to-dnf [expr]
  (dnf/to-dnf expr))

(defn substitute [expr env]
  (ev/substitute expr env))

(defn substitute-and-dnf [expr env]
  (to-dnf (substitute expr env)))