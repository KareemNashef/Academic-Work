#include "signals.h"

int main(int argc, char* argv[]) {

    // CTRL-Signals handling:
    if (signal(SIGTSTP, ctrlZHandler) == SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if (signal(SIGINT, ctrlCHandler) == SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    // SMASH loop:
    SmallShell smash = SmallShell();
    while (true) {
        std::cout << SmashPrompt << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        executeCommand(cmd_line.c_str());
    }
    return 0;
}