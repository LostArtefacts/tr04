    .486
    .model flat

    extern _ACMInit:near
    extern _App:byte
    extern _CheckMMXTechnology:near
    extern _DXCreate:near
    extern _DXDSCreate:near
    extern _DXGetInfo:near
    extern _DXInitKeyboard:near
    extern _DXSetupDialog:near
    extern _Decompress:near
    extern _FindCDDrive:near
    extern _G_dxinfo:byte
    extern _GameMain@4:near
    extern _LoadFile:near
    extern _LoadGameflow:near
    extern _LoadSettings:near
    extern _Log:near
    extern _MainThread:byte
    extern _Var4b44c0:byte
    extern _Var4b44d0:byte
    extern _Var4b44e0:byte
    extern _Var4b44f8:byte
    extern _Var4b4518:byte
    extern _Var4b4540:byte
    extern _Var4b454c:byte
    extern _Var4b4570:byte
    extern _Var533b14:byte
    extern _Var533b18:byte
    extern _WinClose:near
    extern _WinMainWndProc@16:near
    extern _WinProcMsg:near
    extern _WinProcessCommandLine:near
    extern _WinRunCheck:near
    extern __beginthreadex:near
    extern _cutseqpakPtr:byte
    extern _free:near
    extern _gfLanguageFile:byte
    extern _gfScriptFile:byte
    extern _gfStringOffset:byte
    extern _gfStringWad:byte
    extern _malloc:near
    extern _start_setup:byte
    extern _thread_ended:byte
    extern __imp__AdjustWindowRect@12:dword
    extern __imp__ChangeDisplaySettingsA@8:dword
    extern __imp__CreateWindowExA@48:dword
    extern __imp__GetDC@4:dword
    extern __imp__GetDesktopWindow@0:dword
    extern __imp__GetDeviceCaps@8:dword
    extern __imp__GetStockObject@4:dword
    extern __imp__LoadAcceleratorsA@8:dword
    extern __imp__LoadCursorA@8:dword
    extern __imp__MessageBoxA@16:dword
    extern __imp__RegisterClassA@4:dword
    extern __imp__ReleaseDC@8:dword
    extern __imp__SetCursor@4:dword
    extern __imp__SetWindowPos@28:dword
    extern __imp__ShowCursor@4:dword
    extern __imp__ShowWindow@8:dword
    extern __imp__UpdateWindow@4:dword

    .code

_WinMain@16 proc public
    sub esp, 0a8h
    push ebx
    push ebp
    push esi
    xor ebx, ebx
    push edi
    mov byte ptr [_start_setup], bl
    call _CheckMMXTechnology
    test eax, eax
    push offset _App+201
    push offset _Var4b4570
    setne al
    push offset _Var4b454c
    mov byte ptr [_App+231], al
    mov byte ptr [_App+221], bl
    mov dword ptr [_App+244], ebx
    call _WinRunCheck
    add esp, 0ch
    test al, al
    jne near ptr L48ee33
    call _FindCDDrive
    test al, al
    jmp short L48ea3a
L48ea15:
    push 025h
    push offset _Var4b4540
    push offset _Var4b4518
    push ebx
    call dword ptr [__imp__MessageBoxA@16]
    cmp eax, 02h
    je near ptr L48ee33
    call _FindCDDrive
    test al, al
    je short L48ea15
L48ea3a:
    call _LoadGameflow
    mov ecx, dword ptr [esp+0c4h]
    push ecx
    call _WinProcessCommandLine
    mov esi, dword ptr [esp+0c0h]
    add esp, 04h
    mov dword ptr [_App], esi
    mov dword ptr [_App+28], ebx
    push 04h
    mov dword ptr [_App+40], ebx
    mov dword ptr [_App+44], offset _Var4b4570
    call dword ptr [__imp__GetStockObject@4]
    mov edx, dword ptr [_App]
    push 068h
    push edx
    mov dword ptr [_App+36], eax
    mov dword ptr [_App+24], esi
    mov dword ptr [_App+8], 03h
    mov dword ptr [_App+12], offset _WinMainWndProc@16
    mov dword ptr [_App+16], ebx
    mov dword ptr [_App+20], ebx
    call dword ptr [__imp__LoadCursorA@8]
    push offset _App+8
    mov dword ptr [_App+32], eax
    call dword ptr [__imp__RegisterClassA@4]
    test ax, ax
    jne short L48eae7
    push offset _Var4b44f8
    push 01h
    call _Log
    add esp, 08h
    xor eax, eax
    pop edi
    pop esi
    pop ebp
    pop ebx
    add esp, 0a8h
    ret 010h
L48eae7:
    push ebx
    lea eax, [esp+018h]
    push 0c00000h
    push eax
    mov dword ptr [esp+020h], ebx
    mov dword ptr [esp+024h], ebx
    mov dword ptr [esp+028h], 0280h
    mov dword ptr [esp+02ch], 01e0h
    call dword ptr [__imp__AdjustWindowRect@12]
    mov edi, dword ptr [esp+014h]
    push ebx
    mov ecx, dword ptr [esp+024h]
    mov eax, dword ptr [esp+01ch]
    mov edx, dword ptr [esp+020h]
    push esi
    push ebx
    sub ecx, eax
    push ebx
    sub edx, edi
    push ecx
    push edx
    push 080000000h
    push 080000000h
    push 0c00000h
    push offset _Var4b454c
    push offset _Var4b4570
    push 040000h
    call dword ptr [__imp__CreateWindowExA@48]
    cmp eax, ebx
    mov dword ptr [_App+4], eax
    jne short L48eb75
    push offset _Var4b44e0
    push 01h
    call _Log
    add esp, 08h
    xor eax, eax
    pop edi
    pop esi
    pop ebp
    pop ebx
    add esp, 0a8h
    ret 010h
L48eb75:
    push eax
    push offset _App+48
    call _DXGetInfo
    mov al, byte ptr [_start_setup]
    add esp, 08h
    db 03ah, 0c3h
    jne short L48eb95
    call _LoadSettings
    test al, al
    jne short L48ebd1
L48eb95:
    call _DXSetupDialog
    test al, al
    jne short L48ebcc
    mov eax, dword ptr [_gfScriptFile]
    push eax
    call _free
    mov ecx, dword ptr [_gfLanguageFile]
    push ecx
    call _free
    add esp, 08h
    call _WinClose
    pop edi
    pop esi
    pop ebp
    xor eax, eax
    pop ebx
    add esp, 0a8h
    ret 010h
L48ebcc:
    call _LoadSettings
L48ebd1:
    mov edx, dword ptr [_App+153]
    mov eax, dword ptr [_App+149]
    mov ecx, dword ptr [_App+4]
    push 05h
    push ebx
    push ebx
    push edx
    push eax
    push ebx
    push ecx
    call dword ptr [__imp__SetWindowPos@28]
    mov ebp, dword ptr [__imp__GetDesktopWindow@0]
    call ebp
    mov esi, eax
    push esi
    call dword ptr [__imp__GetDC@4]
    mov edi, eax
    push 0ch
    push edi
    call dword ptr [__imp__GetDeviceCaps@8]
    push edi
    push esi
    mov dword ptr [_App+240], eax
    call dword ptr [__imp__ReleaseDC@8]
    mov dword ptr [_App+193], ebx
    mov dword ptr [_App+189], ebx
    mov byte ptr [_App+239], bl
    mov ecx, dword ptr [_G_dxinfo]
    push 0c00000h
    mov edx, dword ptr [ecx+010h]
    mov esi, dword ptr [ecx+08h]
    lea eax, [edx+edx*4]
    shl eax, 05h
    sub eax, edx
    lea edx, [eax+eax*4]
    mov eax, dword ptr [ecx+014h]
    mov ecx, dword ptr [ecx+018h]
    lea edi, [eax+eax*4]
    mov edx, dword ptr [esi+edx*2+0632h]
    lea eax, [eax+edi*8]
    lea edi, [eax+eax*4]
    lea eax, [ecx+ecx*2]
    mov ecx, dword ptr [edx+edi*2+0186h]
    mov edx, dword ptr [_App+4]
    lea eax, [eax+eax*4]
    push edx
    push offset _App+89
    lea eax, [eax+eax*4]
    mov edx, dword ptr [ecx+eax*2+08h]
    lea eax, [ecx+eax*2]
    mov ecx, dword ptr [_App+235]
    push ecx
    mov ecx, dword ptr [eax+04h]
    push edx
    mov edx, dword ptr [eax]
    push ecx
    push edx
    call _DXCreate
    add esp, 01ch
    test eax, eax
    jne short L48eccd
    mov ecx, dword ptr [_gfStringOffset]
    mov edx, dword ptr [_gfStringWad]
    push ebx
    push offset _Var4b44d0
    mov ax, word ptr [ecx+029eh]
    add eax, edx
    push eax
    push ebx
    call dword ptr [__imp__MessageBoxA@16]
    pop edi
    pop esi
    pop ebp
    xor eax, eax
    pop ebx
    add esp, 0a8h
    ret 010h
L48eccd:
    mov edx, dword ptr [_App+4]
    push edx
    call dword ptr [__imp__UpdateWindow@4]
    mov ecx, dword ptr [_App+4]
    mov eax, dword ptr [esp+0c8h]
    push eax
    push ecx
    call dword ptr [__imp__ShowWindow@8]
    test byte ptr [_App+165], 01h
    je short L48ed06
    push ebx
    call dword ptr [__imp__SetCursor@4]
    push ebx
    call dword ptr [__imp__ShowCursor@4]
L48ed06:
    mov edx, dword ptr [_App]
    mov eax, dword ptr [_App+4]
    push edx
    push eax
    call _DXInitKeyboard
    mov ecx, dword ptr [esp+0c4h]
    add esp, 08h
    push 065h
    push ecx
    call dword ptr [__imp__LoadAcceleratorsA@8]
    mov dword ptr [_App+217], eax
    mov al, byte ptr [_App+234]
    db 03ah, 0c3h
    jne short L48ed43
    call _DXDSCreate
    call _ACMInit
L48ed43:
    lea edx, [esp+010h]
    mov dword ptr [_cutseqpakPtr], ebx
    push edx
    push offset _Var4b44c0
    mov dword ptr [esp+018h], ebx
    call _LoadFile
    mov esi, eax
    add esp, 08h
    cmp esi, ebx
    je short L48ed98
    mov eax, dword ptr [esp+010h]
    mov ecx, dword ptr [eax]
    push ecx
    call _malloc
    mov ecx, dword ptr [esp+014h]
    mov dword ptr [_cutseqpakPtr], eax
    add esi, 0fffffffch
    mov edx, dword ptr [ecx]
    add ecx, 04h
    push edx
    push esi
    push ecx
    push eax
    call _Decompress
    mov eax, dword ptr [esp+024h]
    push eax
    call _free
    add esp, 018h
L48ed98:
    mov esi, 01h
    mov dword ptr [_MainThread], esi
    mov dword ptr [_thread_ended], ebx
    push offset _Var533b18
    push ebx
    push ebx
    push offset _GameMain@4
    push ebx
    push ebx
    call __beginthreadex
    add esp, 018h
    mov dword ptr [_Var533b14], eax
    call _WinProcMsg
    mov dword ptr [_thread_ended], esi
    mov eax, dword ptr [_MainThread]
    cmp eax, ebx
    je short L48ede1
L48edd8:
    mov eax, dword ptr [_MainThread]
    cmp eax, ebx
    jne short L48edd8
L48ede1:
    mov eax, dword ptr [_cutseqpakPtr]
    cmp eax, ebx
    je short L48edf3
    push eax
    call _free
    add esp, 04h
L48edf3:
    call _WinClose
    call ebp
    mov esi, eax
    push esi
    call dword ptr [__imp__GetDC@4]
    mov ecx, dword ptr [_App+240]
    push eax
    push esi
    mov word ptr [esp+050h], 094h
    mov dword ptr [esp+094h], ecx
    call dword ptr [__imp__ReleaseDC@8]
    lea edx, [esp+024h]
    push ebx
    push edx
    mov dword ptr [esp+054h], 040000h
    call dword ptr [__imp__ChangeDisplaySettingsA@8]
L48ee33:
    pop edi
    pop esi
    pop ebp
    xor eax, eax
    pop ebx
    add esp, 0a8h
    ret 010h
_WinMain@16 endp

    end
