    .486
    .model flat

    extern @SW_WaitWhileInScene@4:near
    extern _LevelLoadingThread:byte
    extern _LoadLevel@4:near
    extern _Log:near
    extern _Var4b2acc:byte
    extern _Var4b2ad4:byte
    extern __beginthreadex:near
    extern _gfFilenameOffset:byte
    extern _gfFilenameWad:byte

    .code

_S_LoadLevelFile proc public
    sub esp, 050h
    push ebx
    push esi
    push edi
    push offset _Var4b2ad4
    push 02h
    call _Log
    mov eax, dword ptr [esp+068h]
    mov ecx, dword ptr [_gfFilenameOffset]
    xor edi, edi
    lea edx, [esp+014h]
    mov di, word ptr [ecx+eax*2]
    mov eax, dword ptr [_gfFilenameWad]
    add edi, eax
    or ecx, 0ffffffffh
    xor eax, eax
    repnz scasb
    not ecx
    sub edi, ecx
    mov eax, ecx
    mov esi, edi
    mov edi, edx
    lea edx, [esp+014h]
    shr ecx, 02h
    rep movsd
    mov ecx, eax
    xor eax, eax
    and ecx, 03h
    rep movsb
    mov edi, offset _Var4b2acc
    or ecx, 0ffffffffh
    repnz scasb
    not ecx
    sub edi, ecx
    mov esi, edi
    mov ebx, ecx
    mov edi, edx
    or ecx, 0ffffffffh
    repnz scasb
    mov ecx, ebx
    dec edi
    shr ecx, 02h
    rep movsd
    mov ecx, ebx
    and ecx, 03h
    rep movsb
    mov dword ptr [_LevelLoadingThread], 01h
    mov dword ptr [_LevelLoadingThread+8], eax
    push offset _LevelLoadingThread+16
    push eax
    lea eax, [esp+01ch]
    push eax
    push offset _LoadLevel@4
    push 00h
    push 00h
    call __beginthreadex
    add esp, 020h
    mov dword ptr [_LevelLoadingThread+12], eax
    mov eax, dword ptr [_LevelLoadingThread]
    pop edi
    pop esi
    pop ebx
    test eax, eax
    je short L47697d
    db 0e8h, 008h, 090h, 001h, 000h
    nop
    nop
    nop
    nop
L47697d:
    mov eax, 01h
    add esp, 050h
    ret
_S_LoadLevelFile endp

_SW_WaitLevelLoad proc public
    pusha
    mov ecx, offset _LevelLoadingThread
    call @SW_WaitWhileInScene@4
    popa
    ret
_SW_WaitLevelLoad endp

    end
