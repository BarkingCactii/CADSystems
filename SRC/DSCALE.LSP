;Detail Scale
;
;  Scales details withing a baloon to scaled up detail and text
;  Get a circle around entites to br detailed, selects the entities
;  within, and coppies the lot to a selected detail point.
;  

(defun c:dscale
  (
    /
    pnt1	;location of original detail
    pnt2	;location of scaled up detail
    ocircle 	;old circle entity name
    circle 	;circle entity name
    ss		;selection set
    sfactor	;scale factor
    chgblk	;have you exploded blocks
  )
;  (echo 0)
  (setvar "luprec" 4)
  (setvar "orthomode" 0)
;  (k:l901 1 "1C")
  (prompt "\nYou must circle entities to be detailed. ")
;  (setq pnt1 (dgpnt "\nSelect centre point of circle: "))
  (setq pnt1 (getpoint "\nSelect centre point of circle: "))
  (command "circle" pnt1)
  (prompt "\nSelect outer edge of circle: ")
  (command pause)
  (setq
    circle (entlast)
    ss (scircle (entlast))
;    sfactor (dgreal 1.5 1.5 "\nScaling factor for detail: ")
    sfactor (getreal "\nScaling factor for detail: ")
;    pnt2 (dgpnt2 pnt1 "\nSelect insertion point for detail: ")
    pnt2 (getpoint pnt1 "\nSelect insertion point for detail: ")
  )
  (command
    "copy" ss "" pnt1 pnt2
    "copy" circle "" pnt1 pnt2
  )
  (setq
    ocircle circle
    circle (entlast)
    ss (scircle circle)
  )
;  (if (= "Y" (dgopt "N" "" "Y N" 0 "\nScan for blocks, polylines and dimensions (can take a while): "))
  (if (= "y" (getstring "\nScan for blocks, polylines and dimensions (can take a while): "))
    (progn
      (while (= 1 (checkents ss))
        (setq ss (scircle circle) chgblk 1)
      )
      (if chgblk (setq ss (scircle circle)))
    )
  )
  (triments circle ss)
  (command
    "scale" circle ss "" pnt2 sfactor
;    "line" (kdxf 10 (entget ocircle)) (kdxf 10 (entget circle)) ""
    "line" (cdr (assoc 10 (entget ocircle))) (cdr (assoc 10 (entget circle))) ""
    "trim"
      ocircle circle ""
;      (list (entlast) (kdxf 10 (entget ocircle))) 
      (list (entlast) (cdr (assoc 10 (entget ocircle)))) 
;      (list (entlast) (kdxf 10 (entget circle))) 
      (list (entlast) (cdr (assoc 10 (entget circle)))) 
      ""
  )
;  (k:T902 "2CT" "T25")
  (setq
    textp (getpoint "\nSelect point for text (RETURN for default):")
    textp (if textp
            textp
            (polar
;              (kdxf 10 (entget circle))
              (cdr (assoc 10 (entget circle)))
              (dtr 90)
;              (+ (kdxf 40 (entget circle)) (* #SCALE 5))
              (+ (cdr (assoc 40 (entget circle))) 50)
            )
          )
  )
;  (command "text" "c" textp 0 (strcat "%%uDetail Scale 1=" (rtos sfactor 2 2) "%%u"))
;  (echo 1)
)

(defun scircle
  (
    circle	;the circle within which we must search for entities
    /
    circle_g	;the circles data
    cent	;centre of the circle 
    radius	;radius of the circle
    ang		;set angle for selection crossing points
    ss		;the selection set
  )
  (setq
    circle_g (entget circle)
;    cent (kdxf 10 circle_g)
    cent (cdr (assoc 10 circle_g))
;    radius (kdxf 40 circle_g)
    radius (cdr (assoc 40 circle_g))
    ang 0
  )
  (command "select")
  (while (/= ang 90)
    (command
      "c" 
      (polar cent ang radius)
      (polar cent (+ 180 ang) radius)
    )
    (setq ang (+ ang 9))
  )
  (command "")
  (if (setq ss (ssdel circle (ssget "P")))
    ss
  )
)

(defun triments
  (
    circle	;the circle to trim against
    ss		;the selection set of entities to be trimmed
    /
    ssl		;selection set length
    ent		;entity name
    ent_g	;entity data
    ent_t	;entity type
    cent	;centre of circle
    radius	;radius of circle
    circle_g	;data of circle
  )
  (setq
    circle_g (entget circle)
;    cent (kdxf 10 circle_g)
    cent (cdr (assoc 10 circle_g))
;    radius (kdxf 40 circle_g)
    radius (cdr (assoc 40 circle_g))
    count 0
    ssl (sslength ss)
  )
  (setq mmm cent)
  (while (/= count ssl)
    (setq
      ent (ssname ss count)
      ent_g (entget ent)
;      ent_t (kdxf 0 ent_g)
      ent_t (cdr (assoc 0 ent_g))
    )
    (if (= "LINE" ent_t)
      (progn
        (setq
;          end1 (kdxf 10 ent_g)
          end1 (cdr (assoc 10 ent_g))
;          end2 (kdxf 11 ent_g)
          end2 (cdr (assoc 11 ent_g))
          dist1 (distance cent end1)
          dist2 (distance cent end2)
        )
        (if (> dist1 radius)
          (command "trim" circle "" (list ent end1) "")
        )
        (if (> dist2 radius)
          (command "trim" circle "" (list ent end2) "")
        )
      )
    ;else
      (if (or (= ent_t "ARC") (= ent_t "CIRCLE"))
        (command
          "trim" circle ""
          (list
            ent
            (polar
              (kdxf 10 ent_g)
;              (angle (kdxf 10 circle_g) (kdxf 10 ent_g))
              (angle (cdr (assoc 10 circle_g)) (cdr (assoc 10 ent_g)))
              (* 1.5 radius)
            )
          )
          ""
        )
      )
    )
;    (inc 'count)
    (setq count (1+ count))
  )
)

(defun checkents
  (
    ss		;the selection set
    /
    ssl		;selection set length
    count	;counter
    ent		;entity name
    ent_g	;entity data
    ent_t	;entity type
    chg		;return value
  )
  (setq
    count 0
    ssl (sslength ss)
    chg nil
  )
  (princ "\nScanning")
  (while (/= count ssl)
    (setq
      ent (ssname ss count)
      ent_g (entget ent)
;      ent_t (kdxf 0 ent_g)
      ent_t (cdr (assoc 0 ent_g))
    )
    (cond
      ((or (= ent_t "POLYLINE") (= ent_t "DIMENSION"))
        (princ (strcat "\nExploding " (strcase (kdxf 0 ent_g) 1)))
        (command "explode" ent)
        (setq chg 1)
      )
      ((= ent_t "INSERT")
        (princ "\nSkipping block")
      )
    )
;    (inc 'count)
    (setq count (1+ count))
  )
  chg
)
