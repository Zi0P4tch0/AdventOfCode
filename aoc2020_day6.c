#include <glib.h>

static GPtrArray *groups = NULL;

typedef struct {
    GHashTable *answers;
    guint people;
} group_t;

static void group_free(gpointer ptr) {
    if (ptr) {
        group_t *g = ptr;
        g_hash_table_unref(g->answers);
        g_free(g);
    }
}

static group_t *group_new() {
    group_t *g = g_malloc0(sizeof(*g));
    g->answers = g_hash_table_new_full(g_str_hash, g_int_equal, g_free, g_free);
    return g;
}

#define STREQ(L, R) (!g_strcmp0(L, R))

void aoc2020_day6_format_data(const gchar **lines, guint n_lines) {

    groups = g_ptr_array_new_full(0, group_free);
    group_t *g = group_new();

    for (guint i=0; i<n_lines; i++) {
        if (STREQ(*(lines+i), "")) {
            g_ptr_array_add(groups, g);
            g = (i == n_lines - 1 ? NULL : group_new());
        } else {
            for (const gchar *c = *(lines+i); *c != '\0'; c++) {
                gchar *key = g_malloc0(sizeof(*key) * 2);
                strncpy(key, c, 1);
                guint *value = g_malloc0(sizeof(*value)), *existing_value = NULL;
                if ((existing_value = g_hash_table_lookup(g->answers, key)) == NULL) {
                    *value = 1;
                } else {
                    *value = (*existing_value) + 1;
                }
                g_hash_table_insert(g->answers, key, value);
            }
            g->people += 1;
        }
    }

}

void aoc2020_day6_p1(guint n_run) {

    guint sum = 0;

    for (guint i=0; i<groups->len; i++) {
        group_t *g = g_ptr_array_index(groups, i);
        sum += g_hash_table_size(g->answers);
    }

    if (!n_run) g_message("Part I: %d", sum);

}

void aoc2020_day6_p2(guint n_run) {

    guint sum = 0;

    for (guint i=0; i<groups->len; i++) {

        group_t *g = g_ptr_array_index(groups, i);

        GHashTableIter iter;
        gpointer key, value;

        guint questions = 0;

        g_hash_table_iter_init(&iter, g->answers);
        while (g_hash_table_iter_next (&iter, &key, &value)) {
            const guint *v = value;
            if (*v == g->people) { questions += 1; }
        }

        sum += questions;
    }

    if (!n_run) g_message("Part II: %d", sum);

}

void aoc2020_day6_cleanup() {
    g_ptr_array_unref(groups);
}
