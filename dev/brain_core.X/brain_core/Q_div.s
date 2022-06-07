; ===============
; Q_div function
; ===============
; Assembly function performing unsigned integer division on 16 bits words.
; Syntax: uint32 = Q_div(uint16 dividend, uint16 divider)
; The 32 bits output contains the remainder on the 16 LSB and the quotient on the 16 MSB.
.global _Q_div
    
_Q_div:
    MOV W1,W2		; Move divider to W2 (W1 cannot contain the divider)
    REPEAT #17
    DIV.U W0,W2		; Divide W0 by W2
    return
    .end
    
