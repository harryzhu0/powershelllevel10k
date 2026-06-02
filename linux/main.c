#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

#include "pwsh10k.h"
#include "render.h"

#define GRAYSCALE 1
#define RAINBOW 2

void write_pwsh10k_prompt(FILE *pf);
int menu_select(const char *subtitle, const char *items[], int count, int lines);
int quit();

int lines(const char *str) {
    int count = 0;
    if (str == NULL) return 0; // S*fety check

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            count++;
        }
    }
    return count;
}

int installnf() {
    const char* page1_items[] = {
        "0xProto",
        "3270",
        "Adwaita Mono",
        "Agave",
        "Anonymous Pro",
        "Arimo",
        "Atkinson Hyperlegible Mono",
        "Aurulent Sans Mono",
        "BigBlue Terminal",
        "Bitstream Vera Sans Mono",
        "Cascadia Code",
        "Cascadia Mono",
        "Code New Roman",
        "Comic Shanns Mono",
        "Commit Mono",
        "Forward (Page 1 of 5)",
        "Back",
        "quit"
    };

    const char* page2_items[] = {
        "Cousine",
        "D2Coding",
        "DaddyTime Mono",
        "DejaVu Sans Mono",
        "Departure Mono",
        "Droid Sans Mono",
        "Envy Code R",
        "Fantasque Sans Mono",
        "Fira Code",
        "Fira Mono",
        "Font Patcher",
        "Geist Mono",
        "Go Mono",
        "Gohu",
        "Hack",
        "Forward (Page 2 of 5)",
        "Back",
        "quit"
    };

    const char* page3_items[] = {
        "Hasklig",
        "Heavy Data",
        "Hermit",
        "iA Writer",
        "IBM Plex Mono",
        "Inconsolata",
        "Inconsolata Go",
        "Inconsolata LGC",
        "Intel One Mono",
        "Iosevka",
        "Iosevka Term",
        "Iosevka Term Slab",
        "JetBrains Mono",
        "Lekton",
        "Liberation Mono",
        "Forward (Page 3 of 5)",
        "Back",
        "quit"
    };

    const char* page4_items[] = {
        "Lilex",
        "Martian Mono",
        "Meslo",
        "Monaspace",
        "Monofur",
        "Monoid",
        "Mononoki",
        "MPlus",
        "Nerd Fonts Symbols Only",
        "Noto",
        "OpenDyslexic",
        "Overpass",
        "ProFont",
        "ProggyClean",
        "Recursive",
        "Forward (Page 4 of 5)",
        "Back",
        "quit"
    };

    const char* page5_items[]= {
        "Roboto Mono",
        "Share Tech Mono",
        "Source Code Pro",
        "Space Mono",
        "Terminus",
        "Tinos",
        "Ubuntu",
        "Ubuntu Mono",
        "Ubuntu Sans",
        "Victor Mono",
        "Zed Mono",
        "Forward (Page 5 of 5)",
        "Back",
        "quit"
    };

    int page = 1;
    char* target;

    while (1) {
        switch (page) {
                int res = menu_select(
                    "Select a font to install",
                    page1_items, 18, 8
                );
                if (res == 17) {return quit();}
                else if (res == 16) {page = 5;}
                else if (res == 15) {page++;}
                else {target = (char*)page1_items[res];}
            } else if (page == 2) {
                int res = menu_select(
                    "Select a font to install",
                    page2_items, 18, 8
                );
                if (res == 17) {return quit();}
                else if (res == 16) {page--;}
                else if (res == 15) {page++;}
                else {target = (char*)page2_items[res];}
            } else if (page == 3) {
                int res = menu_select(
                    "Select a font to install",
                    page3_items, 18, 8
                );
                if (res == 17) {quit();}
                else if (res == 16) {page--;}
                else if (res == 15) {page++;}
                else {target = (char*)page3_items[res];}
            } else if (page == 4) {
                int res = menu_select(
                    "Select a font to install",
                    page4_items, 18, 8
                );
                if (res == 17) {retur quit();}
                else if (res == 16) {page = 5;}
                else if (res == 15) {page++;}
                else {target = (char*)page4_items[res];}
            5:
                int res = menu_select(
                    "Select a font to install",
                    page5_items, 14, 8
                );
                if (res == 17) {return quit();}
                else if (res == 16) {page--;}
                else if (res == 15) {page = 1;}
                else {target = (char*)page5_items[res];
            }
        }
    }

    char font_url[512];
    snprintf(font_url, sizeof(font_url),
             "https://github.com/ryanoasis/nerd-fonts/releases/latest/download/%s.zip",
             target);

    const char *home = getenv("HOME");
    if (!home) return 1;

    char font_dir[512];
    snprintf(font_dir, sizeof(font_dir), "%s/.local/share/fonts", home);
    mkdir(font_dir, 0755);

    char zip_path[256];
    snprintf(zip_path, sizeof(zip_path), "/tmp/%s.zip", target);

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

int menu_select(const char *subtitle, const char *items[], int count, int lines) {
    int highlight = 0;
    int ch;

    while (1) {
        clear();
        mvfprint(stdscr, 1, 2, "\033[34;1mPowerShell\033[96mLevel\033[2m10K\033[0;22m Installer");
        mvprintw(2, 2, "----------------------------------------");
        mvfprint(stdscr, 4, 2, subtitle);

        int i = 0;

        char buf[512];

        for (i = 0; i < count; i++) {
            if (i == highlight) {
                snprintf(buf, sizeof(buf), "\033[1m> %s\033[22m", items[i]);
                mvfprint(stdscr, lines + i + 5, 0, buf);
            } else {
                mvfprint(stdscr, lines + i + 5, 2, items[i]);
            }
        }

        refresh();

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
    printf("Configuration quited, no changes written to Powershell profile.\n");
    printf("Run \033[36mpwsh10k\033[0m to run the installer again\n");
    return 0;
}

int install_process() {
    int colours = 0;
    int ucode = 0;
    int style = 0;

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
        "quit"
    };

    int s1 = menu_select(
        "Does this look like a diamond?\n\n                --> \033[35m◆\033[0m <--",
        step1_items, 3, 8
    );

    if (s1 == 2) return quit();

    if (s1 == 1) {
        installnf();
    }

    const char *step3_items[] = {
        "Yes, it looks like a lock",
        "No, it does not look like a lock",
        "Restart",
        "quit"
    };

    int s3 = menu_select(

        "Does this look like a lock?\n\n                --> \033[36m\033[0m <--",
        step3_items, 4, 8
    );

    if (s3 == 3) return quit();
    if (s3 == 2) return install_process();

    const char *step4_items[] = {
        "Yes, the columns line up",
        "No, the columns do not line up",
        "Restart",
        "quit"
    };

    int s4 = menu_select(

        "Do the columns line up?\n\n  \033[32mwwwwwwww\n  ||||||||\033[0m",
        step4_items, 4, 9
    );

    if (s4 == 3) return quit();
    if (s4 == 2) return install_process();
    if (s4 == 1) {
        const char *warning[] = {
            "OK"
        };

        int w = menu_select(

            "Monospace fonts are required for pwsh10k to work properly.",
            warning, 1, 6
        );

        installnf();
    }

    const char* step5_items[] = {
        "Yes, the icons are very close to the crosses but there is no overlap.",
        "No, some icons overlap neighbouring crosses.",
        "Restart",
        "quit"
    };

    int s5 = menu_select(
        "Do the icons fit between the crosses\n\n            --->  X\033[31m\033[0mX\033[32m\033[0mX\033[33m\033[0mX\033[34m\033[0mX\033[35m\033[0mX\033[36m\033[0mX\033[37m\033[0mX\033[2;35m\033[0;22mX  <---?",
        step5_items, 4, 8
    );

    if (s5 == 2) return install_process();
    if (s5 == 3) return quit();

    const char* step6_items[] = {
        "Lean        \033[22;0;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m ",
        "Classic     \033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m                                    \033[38;5;8;48;5;48m ✔ \033[38;5;81m\033[48;5;81m 12:53:39 \033[0m",
        "Restart",
        "quit"
    };

    int s6 = menu_select(
        "Pick a theme.",
        step6_items, 4, 6
    );

    if (s6 == 0) {
        const char* step_6a_items[] = {
            "Unicode        \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m",
            "Ascii          \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m>",
            "Restart",
            "quit"
        };

        int s6a = menu_select(
            "Character Set",
            step_6a_items, 4, 6
        );

        if (s6a == 2) return install_process();
        if (s6a == 3) return quit();

        if (s6a == 0) ucode = 1;
        if (s6a == 1) ucode = 0;

        const char* step_6a2_items[] = {
            "256-colour     \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m ",
            "8-colour       \033[22;36m~\033[22m/src\033[22m\033[32m main \033[33m!3 \033[34m?2 \033[0m",
            "Restart",
            "quit"
        };

        int s6a2 = menu_select(
            "Colours",
            step_6a2_items, 4, 6
        );

        if (s6a2 == 2) return install_process();
        if (s6a2 == 3) return quit();

        const char** ptr;

        const char* step_7a_items[] = {
            "12-Hour        \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m                             \033[2m5s 12:24:32 PM\033[22m",
            "24-Hour        \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m                                \033[2m5s 12:24:32\033[22m",
            "No time        \033[22;1;36m~\033[22m/\033[1msrc\033[22m\033[32m main \033[38;5;220m!3 \033[96m?2 \033[0m                                        \033[2m5s \033[22m",
            "Restart",
            "quit"
        };

        const char* step_7b_items[] = {
            "12-Hour        \033[22;36m~\033[22m/src\033[22m\033[32m main \033[33m!3 \033[34m?2 \033[0m                             5s 12:24:32 PM",
            "24-Hour        \033[22;36m~\033[22m/src\033[22m\033[32m main \033[33m!3 \033[34m?2 \033[0m                                5s 12:24:32",
            "No time        \033[22;36m~\033[22m/src\033[22m\033[32m main \033[33m!3 \033[34m?2 \033[0m                                         5s",
            "Restart",
            "quit"
        };

        if (s6a2 == 0) {
            colours = 256;
            ptr = step_7a_items;
        }

        if (s6a2 == 1) {
            colours = 8;
            ptr = step_7b_items;
        }

        int s6a2a = menu_select(
            "Time",
            ptr, 5, 6
        );
    }

    if (s6 == 1) {
        const char* step_6b1_items[] = {
            "Rainbow        \033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m                                    \033[38;5;8;48;5;48m ✔ \033[38;5;81m\033[48;5;81m 12:53:39 \033[0m",
            "Grayscale      \033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m                                    \033[38;5;8;48;5;48m ✔ \033[38;5;81m\033[48;5;81m 12:53:39 \033[0m",
            "Restart",
            "quit"
        };

        int s6b1 = menu_select(
            "Pick a style",
            step_6b1_items, 4, 6
        );

        if (s6b1 == 2) return install_process();
        if (s6b1 == 3) return quit();

        if (s6b1 == 1) style = GRAYSCALE;
        if (s6b1 == 0) style = RAINBOW;

        const char* step_6b1a1_items[] = {
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "Restart",
            "quit"
        };

        int s6b1a1 = menu_select(
            "Pick a prompt",
            step_6b1a1_items, 8, 6
        );

        const char* step_6b1a2_items[] = {
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "\033[22;0;48;5;81;37m ~\033[2m/\033[22;1msrc \033[22;0m\033[38;5;81m\033[48;5;48m \033[32mmain \033[38;5;220m!3 \033[96m?2 \033[0;38;5;48m",
            "Restart",
            "quit"
        };

        int s6b1a2 = menu_select(
            "Pick a colour scheme",
            step_6b1a2_items, 6, 6
        );

    }

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

    install_process();
    endwin();
    printf("\033[?25h");
    return 0;
}
