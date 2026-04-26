#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>
#include <unistd.h>

#include "pwsh10k.h"
#include "render.h"

void write_pwsh10k_prompt(FILE *pf);

int installnf(const char *font) {
    char font_url[512];
    snprintf(font_url, sizeof(font_url),
             "https://github.com/ryanoasis/nerd-fonts/releases/latest/download/%s.zip",
             font);

    const char *home = getenv("HOME");
    if (!home) return 1;

    char font_dir[512];
    snprintf(font_dir, sizeof(font_dir), "%s/.local/share/fonts", home);
    mkdir(font_dir, 0755);

    char zip_path[256];
    snprintf(zip_path, sizeof(zip_path), "/tmp/%s.zip", font);

    char curl_cmd[1024];
    snprintf(curl_cmd, sizeof(curl_cmd),
             "curl -L -o \"%s\" \"%s\"", zip_path, font_url);

    if (system(curl_cmd) != 0) return 1;

    char unzip_cmd[1024];
    snprintf(unzip_cmd, sizeof(unzip_cmd),
             "unzip -o \"%s\" -d \"%s\"", zip_path, font_dir);

    if (system(unzip_cmd) != 0) return 1;

    system("fc-cache -f");
    return 0;
}

int rmkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;

    snprintf(tmp, sizeof(tmp), "%s", dir);

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    return mkdir(tmp, 0755);
}

int menu_select(const char *title, const char *subtitle,
                const char *items[], int count, int oline) {
    int highlight = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(1, 2, "%s", title);
        mvprintw(2, 2, "----------------------------------------");
        mvprintw(4, 2, "%s", subtitle);

        int i = 0;

        char buf[512];

        for (i = 0; i < count; i++) {
            if (i == highlight) {
                snprintf(buf, sizeof(buf), "> %s", items[i]);
                mvfprint(stdscr, oline + i, 0, buf);
            } else {
                mvfprint(stdscr, oline + i, 2, items[i]);
            }
        }


        ch = getch();
        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? count - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == count - 1) ? 0 : highlight + 1;
                break;
            case '\n':
                return highlight;
        }
    }
}

int quit() {
    printf("Configuration aborted, no changes written to Powershell profile.\n");
    printf("Run \033[36mpwsh10k\033[0m to run the installer again\n");
    return 0;
}

void nf_install_process() {
    const char *step2_items[] = {
        "Yes, install Nerd Font",
        "No, skip installation"
    };

    int s2 = menu_select(
        "PowerShell10K Installer",
        "Install Nerd Font?",
        step2_items, 2, 6
    );

    if (s2 == 0) {
        const char *inst_items[] = {
            "JetBrainsMono",
            "FiraCode",
            "Hack",
            "Meslo",
            "GeistMono",
            "Noto",
            "ZedMono"
        };

        int i1 = menu_select(
            "PowerShell10K Installer",
            "Please choose a font",
            inst_items, 7, 6
        );

        installnf(inst_items[i1]);
    } else {
        const char *warn_items[] = { "Yes", "No" };

        int i2 = menu_select(
            "PowerShell10K installer",
            "Warning: PowerShellLevel10K may not work well without nerd fonts.\nContinue installation?",
            warn_items, 2, 7
        );

        if (i2 == 1) quit();
    }
}


int install_wizard() {
    FILE *fp;

    fp = popen("pwsh -noprofile -command 'echo $PROFILE'", "r");
    if (!fp) return 1;

    char profile[256];
    if (!fgets(profile, sizeof(profile), fp)) {
        pclose(fp);
        return 1;
    }
    pclose(fp);

    profile[strcspn(profile, "\r\n")] = 0;

    FILE *pf = fopen(profile, "a");
    if (!pf) {
        rmkdir(profile);
        pf = fopen(profile, "a");
        if (!pf) return 1;
    }

    const char *step1_items[] = {
        "Yes, it looks like a diamond",
        "No, it does not look like a diamond",
        "Abort"
    };

    int s1 = menu_select(
        "PowerShell10K Installer",
        "Does this look like a diamond?\n\n                --> ◆ <--",
        step1_items, 3, 8
    );

    if (s1 == 2) return quit();

    if (s1 == 1) {
        nf_install_process();
    }

    const char *step3_items[] = {
        "Yes, it looks like a lock",
        "No, it does not look like a lock",
        "Restart",
        "Abort"
    };

    int s3 = menu_select(
        "PowerShell10K Installer",
        "Does this look like a lock?\n\n                -->  <--",
        step3_items, 4, 8
    );

    if (s3 == 3) return quit();
    if (s3 == 2) return install_wizard();

    const char *step4_items[] = {
        "Yes, the columns line up",
        "No, the columns do not line up",
        "Restart",
        "Abort"
    };

    int s4 = menu_select(
        "PowerShell10K Installer",
        "Do the columns line up?\n\n  wwwwwwww\n  ||||||||",
        step4_items, 4, 9
    );

    if (s4 == 3) return quit();
    if (s4 == 2) return install_wizard();
    if (s4 == 1) {
        const char *warning[] = {
            "OK"
        };

        int w = menu_select(
            "PowerShell10K Installer",
            "Monospace fonts are required for pwsh10k to work properly.",
            warning, 1, 6
        );

        nf_install_process();
    }

    const char* step5_items[] = {
        "Yes, the icons are very close to the crosses but there is no overlap.",
        "No, some icons overlap neighbouring crosses.",
        "Restart",
        "Abort"
    };

    int s5 = menu_select(
        "PowerShell10K Installer",
        "Do the icons fit between the crosses\n\n            --->  XXXXXXXXX  <---?",
        step5_items, 4, 8
    );

    if (s5 == 2) return install_wizard();
    if (s5 == 3) return quit();

    const char* step6_items[] = {
        "Lean        \033[1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m ",
        "Classic     \033[48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m                                    \033[38;5;8;48;5;48m ✔ \033[38;5;81m\033[48;5;81m 12:53:39 \033[0m",
        "Restart",
        "Abort"
    };

    int s6 = menu_select(
        "PowerShellLevel10K Installer",
        "Pick a theme.",
        step6_items, 4, 6
    );

    clear();
    mvprintw(2, 2, "Writing configuration to PowerShell profile...");

    write_pwsh10k_prompt(pf);

    fclose(pf);

    mvprintw(4, 2, "Done!");
    mvprintw(6, 2, "Press any key to exit.");
    getch();

    return 0;
}

int main() {
    printf("\033[?25l");
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();

    // warm up pair 1 so cache[0] is valid
    init_pair(1, COLOR_WHITE, -1);
    pair_cache[0].fg = COLOR_WHITE;
    pair_cache[0].bg = -1;
    pair_cache[0].id = 1;
    pair_count = 1;

    install_wizard();

    endwin();
    printf("\033[?25h");
    return 0;
}