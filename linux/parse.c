#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *value;
} Variable;

typedef struct {
    char *label;   
    char *target;  
} Choice;

typedef struct {
    int    lines;     
    char   *text; 
    char   *name;      
    Choice *choices;   
    int     nchoices;
} Step;

typedef struct {
    Step *steps;
    int   nsteps; 

    Variable *vars;
    int nvars;
} Program; 

typedef struct {
    int errcode; // if this is 0 then that means that the program ran successfully, probably because you have to set it yourself
    char *err;
    Program prog;
} Return;

static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    
    if (!p) { perror("malloc"); exit(1); }

    memcpy(p, s, len + 1);
    return p;
}

static void add_var(Program *prog, const char *name, const char *value) {
    prog->vars = realloc(prog->vars, sizeof(Variable) * (prog->nvars + 1));

    if (!prog->vars) { perror("realloc"); exit(1); }

    prog->vars[prog->nvars].name = strdup_safe(name);
    prog->vars[prog->nvars].value = strdup_safe(value);
    prog->nvars++;
}

// how do i group this part
static char *trim(char *s) {
    if (!s) return s;
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return s;
}

static void add_choice(Step *step, const char *label, const char *target) {
    step->choices = realloc(step->choices, sizeof(Choice) * (step->nchoices + 1));
    if (!step->choices) { perror("realloc"); exit(1); }

    step->choices[step->nchoices].label  = strdup_safe(label);
    step->choices[step->nchoices].target = strdup_safe(target);
    step->nchoices++;
}

static void add_step(Program *prog, Step *step) {
    prog->steps = realloc(prog->steps, sizeof(Step) * (prog->nsteps + 1));
    if (!prog->steps) { perror("realloc"); exit(1); }

    prog->steps[prog->nsteps] = *step; 
    prog->nsteps++;
    
    memset(step, 0, sizeof(*step));
}

// parser state
enum State {
    STATE_EXPECT_LABEL,
    STATE_EXPECT_BLOCK_START,
    STATE_IN_BLOCK,
    STATE_AFTER_BLOCK
};

Return parse() {
    Return ret = {0};

    FILE *f = fopen("inst.txt", "r");
    if (!f) {
        perror("fopen");
        ret.errcode = 1;
        return ret;
    }

    Program prog = {0};
    Step current = {0};
    enum State state = STATE_EXPECT_LABEL;

    char line[1024];
    char *block_buf = NULL;
    size_t block_cap = 0, block_len = 0;

    while (fgets(line, sizeof(line), f)) {
        char *raw = line;
        
        raw[strcspn(raw, "\r\n")] = 0;

        char *t = trim(raw);

        if (state != STATE_IN_BLOCK && strncmp(t, "var ", 4) == 0) {
            char *p = t + 4;
            char *eq = strchr(p, '=');

            if (!eq) {
                ret.err = "Parse error: '=' expected in declaration";
                ret.errcode = 1;
                return ret;
            }

            *eq = '\0';
            char *name = trim(p);
            char *value = trim(eq + 1);

            size_t L = strlen(value);
            if (L >= 2 && value[0] == '"' && value[L - 1] == '"') {
                value[L - 1] = '\0'; // remove closing quote
                value++;             // skip opening quote
            }

            add_var(&prog, name, value);
            continue;
        }

        if (*t == '\0') {
            if (state == STATE_IN_BLOCK) {
                size_t need = block_len + 1 + 1;

                if (need > block_cap) {
                    block_cap = (need * 2) + 16;
                    block_buf = realloc(block_buf, block_cap);
                    if (!block_buf) { perror("realloc"); exit(1); }
                }

                block_buf[block_len++] = '\n';
                block_buf[block_len] = '\0';
            }
            continue;
        }

        if (state == STATE_EXPECT_LABEL) {
            char *colon = strchr(t, ':');
            if (!colon) {
                ret.err = "Parse error: expected label";
                ret.errcode = 1;
                return ret;
            }

            *colon = '\0';
            current.name = strdup_safe(trim(t));
            state = STATE_EXPECT_BLOCK_START;
            continue;
        }

        if (state == STATE_EXPECT_BLOCK_START) {
            if (strcmp(t, "\"\"\"") != 0) {
                ret.err = "Parse error: expected \"\"\" after label";
                ret.errcode = 1;
                return ret;
            }
            
            free(block_buf);
            block_buf = NULL;
            block_cap = block_len = 0;
            state = STATE_IN_BLOCK;
            continue;
        }

        if (state == STATE_IN_BLOCK) {
            if (strcmp(t, "\"\"\"") == 0) {
                
                if (block_buf == NULL) {
                    current.text = strdup_safe("");
                } else {
                    current.text = malloc(block_len + 1);
                    if (!current.text) { perror("malloc"); exit(1); }
                    memcpy(current.text, block_buf, block_len);
                    current.text[block_len] = '\0';
                }
                state = STATE_AFTER_BLOCK;
                continue;
            }
            
            size_t len = strlen(raw);
            size_t need = block_len + len + 1; 

            if (need + 1 > block_cap) {
                block_cap = (need * 2) + 16;
                block_buf = realloc(block_buf, block_cap);
                if (!block_buf) { perror("realloc"); exit(1); }
            }

            memcpy(block_buf + block_len, raw, len);
            block_len += len;
            block_buf[block_len++] = '\n';
            block_buf[block_len] = '\0';
            continue;
        }

        if (state == STATE_AFTER_BLOCK) {
            if (strchr(t, ':')) {
                
                add_step(&prog, &current);
                state = STATE_EXPECT_LABEL;
                
                char *colon = strchr(t, ':');
                *colon = '\0';
                current.name = strdup_safe(trim(t));
                state = STATE_EXPECT_BLOCK_START;
                continue;
            }

            char *lp = strchr(t, '(');
            char *rp = strchr(t, ')');
            char *arrow = strstr(t, "->");

            if (!lp || !rp || !arrow || rp < lp || arrow < rp) {
                ret.err = "Parse error: expected choice or label";
                ret.errcode = 1;
                return ret;
            }

            *rp = '\0';
            char *choice = trim(lp + 1);

            char *target = arrow + 2;
            target = trim(target);

            add_choice(&current, choice, target);
        }
    }

    if (current.name) {
        add_step(&prog, &current);
    }

    fclose(f);
    free(block_buf);

    ret.prog = prog;
    return ret;
}
