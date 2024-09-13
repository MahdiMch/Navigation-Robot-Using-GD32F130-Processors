
(cl:in-package :asdf)

(defsystem "hugs_control-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "HugsData" :depends-on ("_package_HugsData"))
    (:file "_package_HugsData" :depends-on ("_package"))
  ))