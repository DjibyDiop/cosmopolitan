/*
 * uex_abi_tramp.S
 *
 * Bidirectional Host-to-Guest ABI Trampoline
 * Adapts function calls, stack alignment, and thread-local storage (TLS)
 * between the native Host OS environment and the UEX payload environment.
 */

.text
.globl uex_host_to_guest_tramp
.globl uex_guest_to_host_tramp

/* 
 * Callback from Host OS into UEX Guest (e.g., UI Event Handler)
 * Host OS pushes arguments according to System V or Microsoft x64 ABI.
 * We must save host registers, swap TLS, and jump to UEX handler.
 */
uex_host_to_guest_tramp:
    /* Prologue: Save host state */
    push %rbp
    mov %rsp, %rbp
    
    /* TODO: Save Host %fs / %gs (TLS) and load UEX TLS */
    
    /* Align stack to 16-bytes as per ABI */
    and $-16, %rsp
    
    /* Call the actual UEX function (address passed in a scratch register or memory) */
    /* call *%r11 */
    
    /* Epilogue: Restore host state */
    mov %rbp, %rsp
    pop %rbp
    ret

/*
 * Call from UEX Guest into Host OS (e.g., Native API call)
 * UEX pushes arguments. We must ensure standard alignment, swap TLS, 
 * and execute host function.
 */
uex_guest_to_host_tramp:
    push %rbp
    mov %rsp, %rbp
    
    /* TODO: Save UEX %fs / %gs (TLS) and load Host TLS */
    
    and $-16, %rsp
    
    /* Call the Host function */
    /* call *%r11 */
    
    mov %rbp, %rsp
    pop %rbp
    ret
