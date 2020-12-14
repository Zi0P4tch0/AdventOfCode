#include <glib.h>

typedef struct {
    guint left;
    guint right;
    gchar c;
    gchar *password;
} entry_t;

void free_entry(gpointer data) {
    if (data) {
        entry_t *e = (entry_t *)data;
        g_free(e->password);
        g_free(e);
    }
}

static GPtrArray *entries= NULL;

void aoc2020_day2_format_data(const gchar **lines, guint n_lines) {

    entries = g_ptr_array_new_full(n_lines, free_entry);

    for (guint i=0; i<n_lines; i++) {

        gchar **tokens = g_strsplit(*(lines+i), " ", 0);
        gchar **min_max_tokens = g_strsplit(*tokens, "-", 0);

        entry_t *e = g_new0(entry_t, 1);
        e->left = strtoul(*min_max_tokens, NULL, 10);
        e->right = strtoul(*(min_max_tokens+1), NULL, 10);
        e->c = *(*(tokens+1));
        e->password = g_strdup(*(tokens+2));

        g_ptr_array_add(entries, e);

        g_strfreev(min_max_tokens);
        g_strfreev(tokens);

    }

}

void aoc2020_day2_p1(guint n_run) {

    guint valid = 0;

    for (guint i=0; i<entries->len; i++) {
        entry_t *e = g_ptr_array_index(entries, i);
        guint c_count = 0;
        for (guint x=0; x<strlen(e->password); x++) {
            if (e->password[x] == e->c) c_count += 1;
        }
        if (c_count >= e->left && c_count <= e->right) { valid += 1; }
    }

    if (!n_run) g_message("Part I: %d", valid);
}

void aoc2020_day2_p2(guint n_run) {

    guint valid = 0;

    for (guint i=0; i<entries->len; i++) {
        entry_t *e = g_ptr_array_index(entries, i);
        if ((e->password[e->left-1] == e->c && e->password[e->right-1] != e->c) ||
            (e->password[e->left-1] != e->c && e->password[e->right-1] == e->c)) {
            valid += 1;
        }
    }

    if (!n_run) g_message("Part II: %d", valid);

}

void aoc2020_day2_cleanup() {
    g_ptr_array_unref(entries);
}
