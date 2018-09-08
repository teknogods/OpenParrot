_TEXT SEGMENT

EXTERN originalEP:qword

PUBLIC Main_DoResume
Main_DoResume PROC

and rsp, 0FFFFFFFFFFFFFFF0h
mov rax, 0FFFFFFFFFFFFFFFFh
push rax

mov rax, qword ptr [originalEP]
jmp rax

Main_DoResume ENDP
_TEXT ENDS
END