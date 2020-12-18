#include <argtable3.h>
#include <glib.h>

static void (*years[6 /*years*/ * 25 /* days */ * 4 /*funcs */])();

#define YEARS_INDEX(YEAR, DAY, INDEX) (((YEAR%2015) * 25 * 4) + ((DAY-1) * 4) + INDEX)

#define GEN_DECLS(YEAR, DAY)                                                        \
extern void aoc##YEAR##_day##DAY##_format_data(const gchar **lines, guint n_lines); \
extern void aoc##YEAR##_day##DAY##_p1(guint n_run);                                 \
extern void aoc##YEAR##_day##DAY##_p2(guint n_run);                                 \
extern void aoc##YEAR##_day##DAY##_cleanup();

#define PUSH_DECLS(YEAR, DAY)                                                \
years[YEARS_INDEX(YEAR, DAY, 0)] = aoc##YEAR##_day##DAY##_format_data;       \
years[YEARS_INDEX(YEAR, DAY, 1)] = aoc##YEAR##_day##DAY##_p1;                \
years[YEARS_INDEX(YEAR, DAY, 2)] = aoc##YEAR##_day##DAY##_p2;                \
years[YEARS_INDEX(YEAR, DAY, 3)] = aoc##YEAR##_day##DAY##_cleanup;           \


#define GRAB_DECLS(YEAR, DAY, PART)                                                                     \
void (*fmt)(const gchar**, guint) = (void(*)(const gchar**, guint))years[YEARS_INDEX(YEAR, DAY, 0)];    \
void (*f)(guint) = (void(*)(guint))years[YEARS_INDEX(YEAR, DAY, PART)];                                 \
void (*cleanup)() = (void(*)())years[YEARS_INDEX(YEAR, DAY, 3)];

static struct arg_int *runs;
static struct arg_int *year;
static struct arg_int *day;
static struct arg_int *part;
static struct arg_lit *help;
static struct arg_file *input;
static struct arg_end *end;

void measure(void (*f)(guint)) {
    gint64 best = 0;
    gint64 worst = 0;
    gint64 average = 0;
    for (int i=0; i<runs->ival[0]; i++) {
        gint64 start = g_get_monotonic_time();
        f(i);
        gint64 current = g_get_monotonic_time() - start;
        best = (best == 0 ? current : MIN(best, current));
        worst = MAX(worst, current);
        average += current;
    }
    average /= runs->ival[0];
    if (runs->ival[0] > 1) {
        g_message("best: %lld us | worst: %lld us | average: %lld us.", best, worst, average);
    }
}

GEN_DECLS(2020, 1)
GEN_DECLS(2020, 2)
GEN_DECLS(2020, 3)
GEN_DECLS(2020, 4)
GEN_DECLS(2020, 5)
GEN_DECLS(2020, 6)
GEN_DECLS(2020, 7)
GEN_DECLS(2020, 13)
GEN_DECLS(2020, 14)
GEN_DECLS(2020, 15)
GEN_DECLS(2020, 16)
GEN_DECLS(2020, 18)

int main(int argc, char *argv[]) {

    PUSH_DECLS(2020, 1)
    PUSH_DECLS(2020, 2)
    PUSH_DECLS(2020, 3)
    PUSH_DECLS(2020, 4)
    PUSH_DECLS(2020, 5)
    PUSH_DECLS(2020, 6)
    PUSH_DECLS(2020, 7)
    PUSH_DECLS(2020, 13)
    PUSH_DECLS(2020, 14)
    PUSH_DECLS(2020, 15)
    PUSH_DECLS(2020, 16)
    PUSH_DECLS(2020, 18)

    int exit_code = 0;

    void *argtable[] = {
            runs = arg_int0(NULL, "runs", "<int>", "Activates benchmark mode if greater than 1"),
            year = arg_int1(NULL, "year", "<int>", "Advent Of Code year"),
            day = arg_int1(NULL, "day", "<int>", "Day of the advent"),
            part = arg_int1(NULL, "part", "<int>", "Problem part"),
            help = arg_lit0(NULL,"help", "Print this help and exit"),
            input = arg_file1(NULL, NULL, "<INPUT>", "Input file"),
            end = arg_end(20)
    };

    runs->ival[0] = 1;

    if (arg_nullcheck(argtable)) {
        fprintf(stderr, "argtable3: insufficient memory.\n");
        exit_code=1;
        goto exit;
    }

    int errors = arg_parse(argc, argv, argtable);

    if (help->count > 0) {
        printf("Usage: AdventOfCode");
        arg_print_syntax(stdout, argtable,"\n");
        arg_print_glossary(stdout, argtable,"  %-25s %s\n");
        exit_code = 0;
        goto exit;
    }

    if (errors) {
        arg_print_errors(stdout, end, "AdventOfCode");
        exit_code = 1;
        goto exit;
    }

    GRAB_DECLS(year->ival[0], day->ival[0], part->ival[0]);

    if (!fmt) {
        fprintf(stderr, "Could not find function: aoc%d_day%d_format_data.\n", year->ival[0], day->ival[0]);
        exit_code = 1;
        goto exit;
    }

    if (!f) {
        fprintf(stderr, "Could not find function: aoc%d_day%d_p%d.\n", year->ival[0], day->ival[0], part->ival[0]);
        exit_code = 1;
        goto exit;
    }

    if (!cleanup) {
        fprintf(stderr, "Could not find function: aoc%d_day%d_cleanup.\n", year->ival[0], day->ival[0]);
        exit_code = 1;
        goto exit;
    }

    GError *error = NULL;
    gchar *file = NULL;
    if (!g_file_get_contents(input->filename[0], &file, NULL, &error)) {
        fprintf(stderr, "%s.\n", error->message);
        g_error_free(error);
        exit_code = 1;
    } else {
        gchar **lines = g_strsplit(file, "\n", 0);
        g_free(file);
        guint n_lines = g_strv_length(lines);
        fmt((const gchar**)lines, n_lines);
        measure(f);
        cleanup();
        g_strfreev(lines);
    }

    exit: arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

    return exit_code;
}
