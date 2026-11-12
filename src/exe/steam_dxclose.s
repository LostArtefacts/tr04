    .486
    .model flat

    extern _DXSetCooperativeLevel:near
    extern _G_dxptr:byte
    extern _Log:near
    extern _Var4b2408:byte
    extern _Var4b2428:byte
    extern _Var4bc5c4:byte
    extern _Var4bc614:byte
    extern _Var4bcb00:byte
    extern _Var4bcb10:byte
    extern _Var4bcb1c:byte
    extern _Var4bcb28:byte
    extern _Var4bcb38:byte
    extern _Var4bcb44:byte

    .code

_DXClose proc public
    push esi
    push offset _Var4bcb44
    push 02h
    call _Log
    mov eax, dword ptr [_G_dxptr]
    xor esi, esi
    add esp, 08h
    cmp eax, esi
    je near ptr L493e6b
    mov ecx, dword ptr [eax]
    cmp ecx, esi
    jmp short L493c36
    push 08h
    push esi
    push ecx
    call _DXSetCooperativeLevel
    mov eax, dword ptr [_G_dxptr]
    add esp, 0ch
L493c36:
    mov eax, dword ptr [eax+01ch]
    cmp eax, esi
    je short L493c6d
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+01ch]
    push eax
    push offset _Var4bcb38
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+01ch], esi
    jmp short L493c81
L493c6d:
    push offset _Var4bcb38
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493c81:
    mov edx, dword ptr [_G_dxptr]
    mov eax, dword ptr [edx+08h]
    cmp eax, esi
    je short L493cbe
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+08h]
    push eax
    push offset _Var4bcb28
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+08h], esi
    jmp short L493cd2
L493cbe:
    push offset _Var4bcb28
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493cd2:
    mov edx, dword ptr [_G_dxptr]
    mov eax, dword ptr [edx+018h]
    cmp eax, esi
    je short L493d0f
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+018h]
    push eax
    push offset _Var4bcb1c
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+018h], esi
    jmp short L493d23
L493d0f:
    push offset _Var4bcb1c
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493d23:
    mov edx, dword ptr [_G_dxptr]
    mov eax, dword ptr [edx+014h]
    cmp eax, esi
    je short L493d60
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+014h]
    push eax
    push offset _Var4bcb10
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+014h], esi
    jmp short L493d74
L493d60:
    push offset _Var4bcb10
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493d74:
    mov edx, dword ptr [_G_dxptr]
    mov eax, dword ptr [edx+010h]
    cmp eax, esi
    je short L493db1
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+010h]
    push eax
    push offset _Var4bcb00
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+010h], esi
    jmp short L493dc5
L493db1:
    push offset _Var4bcb00
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493dc5:
    mov eax, dword ptr [_G_dxptr]
    test byte ptr [eax+04ch], 040h
    jne near ptr L493e6b
    mov eax, dword ptr [eax]
    cmp eax, esi
    je short L493e07
    mov edx, dword ptr [eax]
    push eax
    call dword ptr [edx+08h]
    push eax
    mov eax, dword ptr [_G_dxptr]
    mov ecx, dword ptr [eax]
    push ecx
    push offset _Var4bc5c4
    push offset _Var4b2428
    push 04h
    call _Log
    mov edx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [edx], esi
    jmp short L493e1b
L493e07:
    push offset _Var4bc5c4
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493e1b:
    mov eax, dword ptr [_G_dxptr]
    mov eax, dword ptr [eax+04h]
    cmp eax, esi
    je short L493e57
    mov ecx, dword ptr [eax]
    push eax
    call dword ptr [ecx+08h]
    mov edx, dword ptr [_G_dxptr]
    push eax
    mov eax, dword ptr [edx+04h]
    push eax
    push offset _Var4bc614
    push offset _Var4b2428
    push 04h
    call _Log
    mov ecx, dword ptr [_G_dxptr]
    add esp, 014h
    mov dword ptr [ecx+04h], esi
    pop esi
    ret
L493e57:
    push offset _Var4bc614
    push offset _Var4b2408
    push 01h
    call _Log
    add esp, 0ch
L493e6b:
    pop esi
    ret
_DXClose endp

    end
