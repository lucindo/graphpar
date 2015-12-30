; calcula o numero de Stirling de segundo tipo
; formula: http://mathworld.wolfram.com/images/equations/StirlingNumberoftheSecondKind/equation3.gif

; fatorial
(defun ! (n)
  (if (< n 2) 1
    (* n (! (- n 1)))))

; coeficiente binomial
(defun C (n k)
  (/ (! n)
     (* (! k)
	(! (- n k)))))

; numero de stirling de segundo tipo
(defun S (n q)
  (/ (let ((x 0))
       (dotimes (i q)
	 (setf x (+ x (* (expt -1 i) (C q i) (expt (- q i) n)))))
       x)
     (! q)))
