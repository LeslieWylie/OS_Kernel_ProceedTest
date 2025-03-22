#include "printk.h"
#include "keyboard.h"
#include "shell.h"
#include "string.h"

#define VERSION "jiahao@v1.0.0"
#define INPUT_BUFFER_SIZE 300000

void shell() {
    char command[INPUT_BUFFER_SIZE];

    while (1) {
        printk("> "); // 打印提示符
        read_input(command, INPUT_BUFFER_SIZE);

        if (strcmp(command, "version") == 0) {
            printk("%s\n", VERSION);
        } 
        // else {
        //     printk("Unknown command: %s\n", command);
        // }
    }
}
