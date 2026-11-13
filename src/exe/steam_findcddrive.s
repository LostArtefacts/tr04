    .486
    .model flat

    extern _Var4b27a8:byte
    extern _Var4b27ac:byte
    extern _cd_drive:byte
    extern __imp__CloseHandle@4:dword
    extern __imp__CreateFileA@28:dword
    extern __imp__GetDriveTypeA@4:dword
    extern __imp__GetLogicalDrives@0:dword
    extern __imp__lstrcpyA@8:dword

    .code

_FindCDDrive proc public
    sub esp, 018h
    mov eax, dword ptr [_Var4b27ac]
    mov ecx, dword ptr [_Var4b27ac+4]
    mov edx, dword ptr [_Var4b27ac+8]
    push ebx
    mov dword ptr [esp+0ch], eax
    mov ax, word ptr [_Var4b27ac+12]
    push esi
    push edi
    mov dword ptr [esp+018h], ecx
    mov dword ptr [esp+01ch], edx
    mov word ptr [esp+020h], ax
    call dword ptr [__imp__GetLogicalDrives@0]
    lea ecx, [esp+0ch]
    push offset _Var4b27a8
    push ecx
    mov ebx, eax
    mov byte ptr [_cd_drive], 041h
    call dword ptr [__imp__lstrcpyA@8]
    test ebx, ebx
    je short L473cc2
    mov edi, dword ptr [__imp__GetDriveTypeA@4]
    mov esi, dword ptr [__imp__CreateFileA@28]
L473c6d:
    test bl, 01h
    je short L473cb0
    mov dl, byte ptr [_cd_drive]
    lea eax, [esp+0ch]
    push eax
    mov byte ptr [esp+010h], dl
    call edi
    cmp eax, 03h
    jne short L473cb0
    mov cl, byte ptr [_cd_drive]
    push 00h
    push 080h
    push 03h
    push 00h
    push 00h
    lea edx, [esp+028h]
    push 080000000h
    push edx
    mov byte ptr [esp+030h], cl
    call esi
    cmp eax, 0ffffffffh
    jne short L473ccb
L473cb0:
    mov cl, byte ptr [_cd_drive]
    inc cl
    shr ebx, 1
    mov byte ptr [_cd_drive], cl
    jne short L473c6d
L473cc2:
    pop edi
    pop esi
    xor al, al
    pop ebx
    add esp, 018h
    ret
L473ccb:
    push eax
    call dword ptr [__imp__CloseHandle@4]
    pop edi
    pop esi
    mov al, 01h
    pop ebx
    add esp, 018h
    ret
_FindCDDrive endp

    end
