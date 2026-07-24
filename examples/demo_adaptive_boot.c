#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Demonstration of Adaptive Execution Mode
 * The UEX loader calls the appropriate entrypoint based on the environment
 * (e.g. GUI, CLI, or Server mode detected).
 */

void gui_main() {
    printf("[UEX Boot] Started in DESKTOP (GUI) Mode.\n");
    printf("           Initializing Window System...\n");
}

void server_main() {
    printf("[UEX Boot] Started in SERVER Mode.\n");
    printf("           Listening on port 8080...\n");
}

void headless_main() {
    printf("[UEX Boot] Started in EMBEDDED (Headless) Mode.\n");
    printf("           Running background tasks...\n");
}

int main(int argc, char **argv) {
    printf("--- UEX Adaptive Boot Demo ---\n");
    
    // Simulate detecting the execution environment
    const char *env_mode = getenv("UEX_FORCE_MODE");
    
    if (env_mode != NULL && strcmp(env_mode, "server") == 0) {
        server_main();
    } else if (env_mode != NULL && strcmp(env_mode, "headless") == 0) {
        headless_main();
    } else {
        // Defaulting to GUI / Desktop
        gui_main();
    }

    return 0;
}
