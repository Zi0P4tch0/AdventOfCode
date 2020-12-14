#include <glib.h>

extern void combinations(const gint *array, guint size, guint k, void (*on_found)(const gint*, guint), guint n_run);

static GArray *numbers = NULL;

void aoc2020_day1_format_data(const gchar **lines, guint n_lines) {
    numbers = g_array_sized_new(FALSE, FALSE, sizeof(gint), n_lines);
    for (guint i=0; i<n_lines; i++) {
        gint value = strtol(*(lines+i), NULL, 10);
        g_array_append_val(numbers, value);
    }
}

void aoc2020_day1_p1(guint n_run) {
    g_autoptr(GHashTable) table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    for (guint i=0; i<numbers->len; i++) {
        gint sum_minus_element = 2020 - g_array_index(numbers, gint, i);
        if (g_hash_table_lookup(table, GINT_TO_POINTER(sum_minus_element))) {
            if (!n_run) g_message("Part I: %d", g_array_index(numbers, gint, i) * sum_minus_element);
            break;
        }
        g_hash_table_insert(table, GINT_TO_POINTER(g_array_index(numbers, gint, i)), GINT_TO_POINTER(1));
    }
}

static void on_threesome_found(const gint* combination, guint n_run) {
    if (combination[0] + combination[1] + combination[2] == 2020) {
        if (!n_run) g_message("Part II: %d", combination[0] * combination[1] * combination[2]);
    }
}

void aoc2020_day1_p2(guint n_run) {
    combinations((const gint*)numbers->data, numbers->len, 3, &on_threesome_found, n_run);
}

void aoc2020_day1_cleanup() {
    g_array_unref(numbers);
}
