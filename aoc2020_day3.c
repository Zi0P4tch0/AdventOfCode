#include <glib.h>

typedef struct {
    guint right;
    guint down;
} slope_t;

static const gchar **map = NULL;
static guint map_height = 0;

void aoc2020_day3_format_data(const gchar **lines, guint n_lines) {
    map = lines;
    map_height = n_lines;
}

guint trees(slope_t slope) {
    guint row = slope.down, column = slope.right, trees = 0;
    guint map_width = strlen(*map);
    while (row < map_height) {
        if (map[row][column % map_width] == '#') {
            trees += 1;
        }
        row += slope.down;
        column += slope.right;
    }
    return trees;
}

void aoc2020_day3_p1(guint n_run) {

    slope_t s = { 3, 1 };
    if (!n_run) g_message("Part I: %d", trees(s));
}

void aoc2020_day3_p2(guint n_run) {

    slope_t slopes[] = {
            { 1, 1},
            { 3, 1 },
            { 5, 1 },
            { 7, 1 },
            { 1, 2 },
    };

    guint64 t = 1;

    for (guint i=0; i<sizeof(slopes) / sizeof(slopes[0]); i++) {
        t *= trees(slopes[i]);
    }

    if (!n_run) g_message("Part II: %lld", t);
}

void aoc2020_day3_cleanup() { }
