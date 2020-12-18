#include <glib.h>

const gchar **expressions = NULL;
guint n_expressions = 0;

void aoc2020_day18_format_data(const gchar **lines, guint n_lines) {
    expressions = lines;
    n_expressions = n_lines;
}

static
gboolean g_char_array_contains(GArray *a, gchar c, guint *index) {
    for (guint i=0; i<a->len; i++) {
        if (g_array_index(a, gchar, i) == c) {
            *index = i;
            return TRUE;
        }
    }
    return FALSE;
}

static
guint64 solve(gchar *expression, gboolean prioritise_addition) {

    g_autoptr(GArray) values = g_array_new(FALSE, FALSE, sizeof(guint64));
    g_autoptr(GArray) operators = g_array_new(FALSE, FALSE, sizeof(gchar));

    gchar *it = expression;
    while (*it != '\0') {
        if (*it == ' ') { goto next; }
        if (*it == '+' ||  *it == '*') {
            gchar op = *it;
            g_array_append_val(operators, op);
            goto next;
        }
        if (g_ascii_isdigit(*it)) {
            gchar *next_it = NULL;
            guint64 number = strtoull(it, &next_it, 10);
            g_array_append_val(values, number);
            it = next_it;
            goto next_no_inc;
        }
        if (*it == '(') {
            guint lpar = 1;
            gchar *start = it+1;
            gchar *end = it;
            while (lpar > 0) {
                end++;
                if (*end == ')') lpar--;
                if (*end == '(') lpar++;
            }
            g_autofree gchar *sub = g_strndup(start, end-start);
            guint64 result = solve(sub, prioritise_addition);
            g_array_append_val(values, result);
            it = end+1;
            goto next_no_inc;
        }
        next: it += 1;
        next_no_inc:;

    }

    if (prioritise_addition) {
        guint i=0;
        while(g_char_array_contains(operators, '+', &i)) {
            guint64 lhs = g_array_index(values, guint64, i);
            guint64 rhs = g_array_index(values, guint64, i+1);
            guint64 result = lhs+rhs;
            values = g_array_remove_range(values, i, 2);
            operators = g_array_remove_index(operators, i);
            g_array_insert_val(values, i, result);
        }
    }

    while (values->len > 1) {

        guint64 lhs = g_array_index(values, guint64, 0);
        guint64 rhs = g_array_index(values, guint64, 1);
        gchar op = g_array_index(operators, gchar, 0);

        guint64 result = (op == '+' ? lhs+rhs : lhs*rhs);

        values = g_array_remove_range(values, 0, 2);
        operators = g_array_remove_index(operators, 0);

        g_array_insert_val(values, 0, result);

    }

    return g_array_index(values, guint64, 0);
}

void aoc2020_day18_p1(guint n_run) {

    guint64 sum = 0;

    for (guint i=0; i<n_expressions; i++) {
        sum += solve((gchar *) *(expressions + i), FALSE);
    }

    if (!n_run) g_message("Part I: %lld", sum);

}

void aoc2020_day18_p2(guint n_run) {

    guint64 sum = 0;

    for (guint i=0; i<n_expressions; i++) {
        sum += solve((gchar *) *(expressions + i), TRUE);
    }

    if (!n_run) g_message("Part II: %lld", sum);

}

void aoc2020_day18_cleanup() {

}
