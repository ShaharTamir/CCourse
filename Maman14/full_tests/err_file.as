; file ps.as
.entry LIST

; invalid name because of 5 at start, chars that are not number and alphabet, length
.entry 5INVALID_NAME&legnjkhasdfoijalt12387234mkasdflkj

.extern W

; add has too many ','
MAIN:   add r3,   ,     LIST

LOOP:   prn #48
        inc r6
        mov r3  ,       W
        lea STR,r6
        sub r1,r4
        bne END
        cmp val1, #-6

        ; no ',' between END and r15
        bne END r15

        dec K
.entry MAIN
        ; using invalid indexes
        sub LOOP[r9]   ,LOOP[r16]

; END is defined twice
END:    stop
END:    .string "ab"

; empty label name
:       stop

STR:    .string "abcd"
LIST:   .data 6, -9
        .data  -100
.entry K
K:      .data 40012331                                                          1234
.extern val1
