    .486
    .model flat

    extern @SW_WaitUntilInScene@4:near
    extern @SW_WaitWhileInScene@4:near
    extern _App:byte
    extern _DXMove:near
    extern _FillADPCMBuffer:near
    extern _Log:near
    extern _Var4b45a4:byte
    extern _Var4b45b8:byte
    extern _Var4b45d0:byte
    extern _Var4b45f4:byte
    extern _Var4b4600:byte
    extern _Var4b460c:byte
    extern _Var4b4618:byte
    extern _Var4b4620:byte
    extern _Var4b462c:byte
    extern _Var4b4638:byte
    extern _Var533b14:byte
    extern _Var753968:byte
    extern _Var75396c:byte
    extern _Var753970:byte
    extern _Var753c24:byte
    extern _WinProcessCommands:near
    extern _resChangeCounter:byte
    extern __imp__DefWindowProcA@16:dword
    extern __imp__PostQuitMessage@4:dword
    extern __imp__ResumeThread@4:dword
    extern __imp__SuspendThread@4:dword

    .code

_WinMainWndProc@16 proc public
    push ebx
    mov ebx, dword ptr [esp+010h]
    push esi
    mov esi, dword ptr [esp+010h]
    push edi
    mov edi, dword ptr [esp+01ch]
    cmp esi, 010h
    ja near ptr L48f60b
    cmp esi, 010h
    je near ptr L48f5d9
    mov eax, esi
    dec eax
    je near ptr L48f5ac
    sub eax, 02h
    je near ptr L48f577
    sub eax, 03h
    jne near ptr L48f68a
    push offset _Var4b4638
    push 06h
    call _Log
    mov al, byte ptr [_Var753c24]
    add esp, 08h
    test al, al
    jne near ptr L48f68a
    mov al, byte ptr [_App+239]
    test al, al
    je short L48f499
    pop edi
    pop esi
    xor eax, eax
    pop ebx
    ret 010h
L48f499:
    mov eax, dword ptr [esp+018h]
    and eax, 0ffffh
    je short L48f4fe
    test eax, eax
    jle near ptr L48f56f
    cmp eax, 02h
    jg near ptr L48f56f
    push offset _Var4b462c
    push 06h
    call _Log
    mov al, byte ptr [_App+221]
    add esp, 08h
    test al, al
    je near ptr L48f56f
    mov eax, dword ptr [_Var533b14]
    push eax
    call dword ptr [__imp__ResumeThread@4]
    mov dword ptr [_App+193], 00h
    push offset _Var4b45a4
    push 05h
    call _Log
    add esp, 08h
    xor eax, eax
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f4fe:
    push offset _Var4b4620
    push 06h
    call _Log
    mov al, byte ptr [_App+221]
    add esp, 08h
    test al, al
    je short L48f56f
    push offset _Var4b45d0
    push 05h
    call _Log
    add esp, 08h
    mov eax, dword ptr [_App+189]
    test eax, eax
    je short L48f537
    db 0e8h, 08eh, 004h, 000h, 000h
    nop
    nop
    nop
    nop
L48f537:
    mov dword ptr [_App+193], 01h
    mov eax, dword ptr [_App+189]
    test eax, eax
    jne short L48f553
    db 0e8h, 052h, 004h, 000h, 000h
    nop
    nop
    nop
    nop
L48f553:
    mov ecx, dword ptr [_Var533b14]
    push ecx
    call dword ptr [__imp__SuspendThread@4]
    push offset _Var4b45b8
    push 05h
    call _Log
    add esp, 08h
L48f56f:
    pop edi
    pop esi
    xor eax, eax
    pop ebx
    ret 010h
L48f577:
    push offset _Var4b4618
    push 06h
    call _Log
    mov edx, edi
    shr edx, 010h
    movsx eax, dx
    movsx ecx, di
    push eax
    push ecx
    call _DXMove
    mov edx, dword ptr [esp+020h]
    add esp, 010h
    push edi
    push ebx
    push esi
    push edx
    call dword ptr [__imp__DefWindowProcA@16]
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f5ac:
    push offset _Var4b460c
    push 06h
    mov dword ptr [_resChangeCounter], 00h
    call _Log
    mov edx, dword ptr [esp+018h]
    add esp, 08h
    push edi
    push ebx
    push esi
    push edx
    call dword ptr [__imp__DefWindowProcA@16]
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f5d9:
    push offset _Var4b4600
    push 06h
    call _Log
    add esp, 08h
    mov byte ptr [_Var753c24], 01h
    push 00h
    call dword ptr [__imp__PostQuitMessage@4]
    push edi
    push ebx
    mov edx, dword ptr [esp+018h]
    push esi
    push edx
    call dword ptr [__imp__DefWindowProcA@16]
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f60b:
    cmp esi, 0111h
    je short L48f66d
    cmp esi, 0200h
    je short L48f63d
    cmp esi, 08000h
    jne short L48f68a
    mov edx, dword ptr [esp+018h]
    mov eax, dword ptr [esp+01ch]
    push edx
    push eax
    call _FillADPCMBuffer
    add esp, 08h
    xor eax, eax
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f63d:
    mov edx, edi
    push edi
    shr edx, 010h
    movsx ecx, di
    movsx eax, dx
    mov edx, dword ptr [esp+014h]
    push ebx
    push esi
    push edx
    mov dword ptr [_Var75396c], ecx
    mov dword ptr [_Var753968], eax
    mov dword ptr [_Var753970], ebx
    call dword ptr [__imp__DefWindowProcA@16]
    pop edi
    pop esi
    pop ebx
    ret 010h
L48f66d:
    push offset _Var4b45f4
    push 06h
    call _Log
    mov ecx, ebx
    and ecx, 0ffffh
    push ecx
    call _WinProcessCommands
    add esp, 0ch
L48f68a:
    mov edx, dword ptr [esp+010h]
    push edi
    push ebx
    push esi
    push edx
    call dword ptr [__imp__DefWindowProcA@16]
    pop edi
    pop esi
    pop ebx
    ret 010h
_WinMainWndProc@16 endp

_SW_WaitUntilDXInScene proc public
    pusha
    mov ecx, offset _App+189
    call @SW_WaitUntilInScene@4
    popa
    ret
_SW_WaitUntilDXInScene endp

_SW_WaitWhileDXInScene proc public
    pusha
    mov ecx, offset _App+189
    call @SW_WaitWhileInScene@4
    popa
    ret
_SW_WaitWhileDXInScene endp

    end
