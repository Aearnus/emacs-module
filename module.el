(defun list-functions () (cl-loop for s being the symbols if (fboundp s) collect (symbol-name s)))
(defun list-symbols () (cl-loop for s being the symbols if (boundp s) collect (symbol-name s)))

(require 'luajit)
(provide 'module)
