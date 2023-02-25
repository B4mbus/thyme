(fn first [elems] (. elems 1))
(fn second [elems] (. elems 2))

(fn split-by-space [string]
  (icollect [word (string.gmatch string "%S+")] word))

(fn luajit-version []
  (-?> _G.jit
       (. :version)
       (split-by-space)
       (first)))

(fn lua-version []
  (-> _VERSION
      (split-by-space)
      (second)))

(print (. (require :fennel) :version) (or (luajit-version) (lua-version)))
