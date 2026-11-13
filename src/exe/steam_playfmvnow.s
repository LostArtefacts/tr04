    .486
    .model flat

    extern _App:byte
    extern _Bink:byte
    extern _BinkSurface:byte
    extern _BinkSurfaceType:byte
    extern _ClearSurfaces:near
    extern _CreateD3DLights:near
    extern _DXChangeOutputFormat:near
    extern _DXChangeVideoMode:near
    extern _FmvSceneTriggered:byte
    extern _FreeD3DLights:near
    extern _G_dxinfo:byte
    extern _HWInitialise:near
    extern _InitFont:near
    extern _InitWindow:near
    extern _S_CDStop:near
    extern _S_InitD3DMatrix:near
    extern _S_UpdateInput:near
    extern _SetD3DViewMatrix:near
    extern _ShowBinkFrame:near
    extern _SoundQuality:byte
    extern _Var4bcde8:byte
    extern _fmvs_disabled:byte
    extern _input:byte
    extern _sfx_frequencies:byte
    extern _sprintf:near
    extern _thread_ended:byte
    extern __imp___BinkClose@4:dword
    extern __imp___BinkDDSurfaceType@4:dword
    extern __imp___BinkDoFrame@4:dword
    extern __imp___BinkNextFrame@4:dword
    extern __imp___BinkOpen@8:dword
    extern __imp___BinkOpenDirectSound@4:dword
    extern __imp___BinkSetSoundSystem@8:dword
    extern __imp___BinkWait@4:dword

    .code

_PlayFmvNow proc public
    sub esp, 0a0h
    push esi
    mov eax, dword ptr [_thread_ended]
    test eax, eax
    je short L49c3fa
    xor eax, eax
    pop esi
    add esp, 0a0h
    ret
L49c3fa:
    mov esi, dword ptr [esp+0a8h]
    mov eax, 01h
    mov ecx, esi
    shl eax, cl
    test dword ptr [_FmvSceneTriggered], eax
    je short L49c41f
    mov eax, 01h
    pop esi
    add esp, 0a0h
    ret
L49c41f:
    or dword ptr [_FmvSceneTriggered], eax
    call _S_CDStop
    mov al, byte ptr [_fmvs_disabled]
    test al, al
    je short L49c43d
    xor eax, eax
    pop esi
    add esp, 0a0h
    ret
L49c43d:
    push ebx
    push ebp
    push edi
    push esi
    lea eax, [esp+064h]
    push offset _Var4bcde8
    push eax
    call _sprintf
    mov ecx, 014h
    xor eax, eax
    lea edi, [esp+01ch]
    add esp, 0ch
    rep stosd
    mov cl, 00h
    nop
    nop
    nop
    nop
    lea edi, [esp+060h]
    mov byte ptr [esp+010h], cl
    or ecx, 0ffffffffh
    repnz scasb
    not ecx
    sub edi, ecx
    lea edx, [esp+010h]
    mov esi, edi
    mov ebx, ecx
    mov edi, edx
    or ecx, 0ffffffffh
    mov byte ptr [esp+011h], 00h
    mov byte ptr [esp+012h], 00h
    repnz scasb
    mov ecx, ebx
    dec edi
    shr ecx, 02h
    rep movsd
    mov ecx, ebx
    and ecx, 03h
    rep movsb
    mov byte ptr [_App+239], 01h
    mov ecx, dword ptr [_App+64]
    mov esi, dword ptr [_G_dxinfo]
    mov ebp, dword ptr [_App+72]
    lea eax, [ecx+ecx*4]
    xor ebx, ebx
    shl eax, 05h
    sub eax, ecx
    mov edx, dword ptr [esi+08h]
    lea ecx, [eax+eax*4]
    mov eax, dword ptr [_App+68]
    mov ecx, dword ptr [edx+ecx*2+0632h]
    lea edi, [eax+eax*4]
    lea eax, [eax+edi*8]
    lea eax, [eax+eax*4]
    mov edi, dword ptr [ecx+eax*2+0186h]
    lea eax, [ebp+ebp*2+00h]
    lea eax, [eax+eax*4]
    lea eax, [eax+eax*4]
    mov ecx, dword ptr [edi+eax*2+08h]
    lea eax, [edi+eax*2]
    cmp ecx, 020h
    jne short L49c50b
    cmp dword ptr [eax], 0280h
    jne short L49c50b
    cmp dword ptr [eax+04h], 01e0h
    je short L49c57c
L49c50b:
    mov ecx, dword ptr [esi+010h]
    xor ebx, ebx
    lea eax, [ecx+ecx*4]
    shl eax, 05h
    sub eax, ecx
    lea ecx, [eax+eax*4]
    mov eax, dword ptr [esi+014h]
    mov ecx, dword ptr [edx+ecx*2+0632h]
    lea esi, [eax+eax*4]
    lea eax, [eax+esi*8]
    lea eax, [eax+eax*4]
    mov eax, dword ptr [ecx+eax*2+0182h]
    test eax, eax
    jle short L49c563
L49c539:
    cmp dword ptr [edi+08h], 020h
    jne short L49c550
    cmp dword ptr [edi], 0280h
    jne short L49c550
    cmp dword ptr [edi+04h], 01e0h
    je short L49c55d
L49c550:
    inc ebx
    add edi, 096h
    cmp ebx, eax
    jl short L49c539
    jmp short L49c563
L49c55d:
    mov dword ptr [_App+72], ebx
L49c563:
    call _FreeD3DLights
    call _DXChangeVideoMode
    call _HWInitialise
    call _ClearSurfaces
    mov ebx, 01h
L49c57c:
    mov edx, dword ptr [_App+121]
    mov eax, dword ptr [__imp___BinkOpenDirectSound@4]
    xor esi, esi
    push edx
    push eax
    mov dword ptr [_Bink], esi
    call dword ptr [__imp___BinkSetSoundSystem@8]
    lea ecx, [esp+010h]
    push esi
    push ecx
    call dword ptr [__imp___BinkOpen@8]
    mov dword ptr [_Bink], eax
    mov al, byte ptr [_App+165]
    test al, 02h
    mov eax, dword ptr [_App+109]
    jne short L49c5bb
    mov eax, dword ptr [_App+105]
L49c5bb:
    push eax
    mov dword ptr [_BinkSurface], eax
    call dword ptr [__imp___BinkDDSurfaceType@4]
    mov dword ptr [_BinkSurfaceType], eax
    mov eax, dword ptr [_Bink]
    cmp eax, esi
    je near ptr L49c65b
    push eax
    call dword ptr [__imp___BinkDoFrame@4]
    call _S_UpdateInput
    mov eax, dword ptr [_Bink]
    cmp dword ptr [eax+010h], esi
    je short L49c64c
    mov edi, dword ptr [__imp___BinkWait@4]
L49c5f5:
    mov ecx, dword ptr [_input]
    test ch, 01h
    jne short L49c64c
    mov ecx, dword ptr [_thread_ended]
    test ecx, ecx
    jne short L49c64c
    push eax
    call dword ptr [__imp___BinkNextFrame@4]
    mov edx, dword ptr [_Bink]
    push edx
    call edi
    test eax, eax
    je short L49c62a
L49c61e:
    mov eax, dword ptr [_Bink]
    push eax
    call edi
    test eax, eax
    jne short L49c61e
L49c62a:
    call _ShowBinkFrame
    mov ecx, dword ptr [_Bink]
    inc esi
    push ecx
    call dword ptr [__imp___BinkDoFrame@4]
    call _S_UpdateInput
    mov eax, dword ptr [_Bink]
    cmp esi, dword ptr [eax+010h]
    jne short L49c5f5
L49c64c:
    push eax
    call dword ptr [__imp___BinkClose@4]
    xor esi, esi
    mov dword ptr [_Bink], esi
L49c65b:
    cmp ebx, esi
    je short L49c6a0
    mov dword ptr [_App+72], ebp
    call _DXChangeVideoMode
    call _CreateD3DLights
    mov eax, dword ptr [_App+129]
    mov ecx, dword ptr [_App+125]
    push eax
    push ecx
    push 050h
    push 05000h
    push 014h
    push eax
    push ecx
    push esi
    push esi
    call _InitWindow
    add esp, 024h
    call _InitFont
    call _S_InitD3DMatrix
    call _SetD3DViewMatrix
L49c6a0:
    mov edx, dword ptr [_SoundQuality]
    push 01h
    mov eax, dword ptr [edx*4+offset _sfx_frequencies]
    push eax
    call _DXChangeOutputFormat
    add esp, 08h
    call _HWInitialise
    call _ClearSurfaces
    mov byte ptr [_App+239], 00h
    pop edi
    pop ebp
    pop ebx
    xor eax, eax
    pop esi
    add esp, 0a0h
    ret
_PlayFmvNow endp

    end
