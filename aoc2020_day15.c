#include <glib.h>

static GArray *numbers = NULL;

void aoc2020_day15_format_data(const gchar **lines, guint n_lines) {
    g_assert(n_lines == 1);
    numbers = g_array_new(FALSE, FALSE, sizeof(guint));
    gchar **tokens = g_strsplit(*lines, ",", 0);
    for (guint i=0; i<g_strv_length(tokens); i++) {
        guint value = strtoul(*(tokens+i), NULL, 10);
        g_array_append_val(numbers, value);
    }
    g_strfreev(tokens);
}

guint van_ecks_nth(const GArray *seed, guint nth) {
    g_autoptr(GHashTable) m = g_hash_table_new(g_direct_hash, g_direct_equal);
    // Populate hash table with seed
    for (guint i=0; i<seed->len; i++) {
        guint key = g_array_index(seed, guint, i);
        guint value = i+1;
        g_hash_table_insert(m, GUINT_TO_POINTER(key), GUINT_TO_POINTER(value));
    }
    // Iteration
    guint last = g_array_index(seed, guint, seed->len-1);
    for (guint n=seed->len; n<nth; n++) {
        gpointer *last_seen = g_hash_table_lookup(m, GUINT_TO_POINTER(last));
        guint next = 0;
        if (last_seen != NULL) {
            next = n - GPOINTER_TO_UINT(last_seen);
        } else {
            next = 0;
        }
        g_hash_table_insert(m, GUINT_TO_POINTER(last), GUINT_TO_POINTER(n));
        last = next;
    }
    return last;
}

void aoc2020_day15_p1(guint n_run) {
    guint result = van_ecks_nth(numbers, 2020);
    if (!n_run) g_message("Part I: %u", result);
}

void aoc2020_day15_p2(guint n_run) {
    guint result = van_ecks_nth(numbers, 30000000);
    if (!n_run) g_message("Part II: %u", result);
}

void aoc2020_day15_cleanup() {
    g_array_unref(numbers);
}
