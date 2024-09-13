;; Auto-generated. Do not edit!


(when (boundp 'hugs_control::HugsData)
  (if (not (find-package "HUGS_CONTROL"))
    (make-package "HUGS_CONTROL"))
  (shadow 'HugsData (find-package "HUGS_CONTROL")))
(unless (find-package "HUGS_CONTROL::HUGSDATA")
  (make-package "HUGS_CONTROL::HUGSDATA"))

(in-package "ROS")
;;//! \htmlinclude HugsData.msg.html


(defclass hugs_control::HugsData
  :super ros::object
  :slots (_data ))

(defmethod hugs_control::HugsData
  (:init
   (&key
    ((:data __data) (make-array 0 :initial-element 0 :element-type :char))
    )
   (send-super :init)
   (setq _data __data)
   self)
  (:data
   (&optional __data)
   (if __data (setq _data __data)) _data)
  (:serialization-length
   ()
   (+
    ;; uint8[] _data
    (* 1    (length _data)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint8[] _data
     (write-long (length _data) s)
     (princ _data s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint8[] _data
   (let ((n (sys::peek buf ptr- :integer))) (incf ptr- 4)
     (setq _data (make-array n :element-type :char))
     (replace _data buf :start2 ptr-) (incf ptr- n))
   ;;
   self)
  )

(setf (get hugs_control::HugsData :md5sum-) "f43a8e1b362b75baa741461b46adc7e0")
(setf (get hugs_control::HugsData :datatype-) "hugs_control/HugsData")
(setf (get hugs_control::HugsData :definition-)
      "uint8[] data  # Array of bytes representing the data buffer


")



(provide :hugs_control/HugsData "f43a8e1b362b75baa741461b46adc7e0")


