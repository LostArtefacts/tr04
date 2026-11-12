    .486
    .model flat

    extern _Log:near
    extern _Var4b27bc:byte
    extern _Var4b27d0:byte
    extern _Var4b27d4:byte
    extern _fopen:near

    .code

_FileOpen proc public
    sub esp, 050h
    push ebx
    push esi
    push edi
    mov ecx, 014h
    xor eax, eax
    lea edi, [esp+0ch]
    rep stosd
    mov al, 00h
    nop
    nop
    nop
    mov edi, dword ptr [esp+060h]
    mov byte ptr [esp+0ch], al
    or ecx, 0ffffffffh
    xor eax, eax
    lea edx, [esp+0ch]
    repnz scasb
    not ecx
    sub edi, ecx
    mov byte ptr [esp+0dh], 00h
    mov esi, edi
    mov ebx, ecx
    mov edi, edx
    or ecx, 0ffffffffh
    mov byte ptr [esp+0eh], 00h
    repnz scasb
    mov ecx, ebx
    dec edi
    shr ecx, 02h
    rep movsd
    mov ecx, ebx
    lea eax, [esp+0ch]
    and ecx, 03h
    push eax
    push offset _Var4b27d4
    push 05h
    rep movsb
    call _Log
    lea ecx, [esp+018h]
    push offset _Var4b27d0
    push ecx
    call _fopen
    mov esi, eax
    add esp, 014h
    test esi, esi
    jne short L473d70
    lea edx, [esp+0ch]
    push edx
    push offset _Var4b27bc
    push 01h
    call _Log
    add esp, 0ch
L473d70:
    mov eax, esi
    pop edi
    pop esi
    pop ebx
    add esp, 050h
    ret
_FileOpen endp

    end
