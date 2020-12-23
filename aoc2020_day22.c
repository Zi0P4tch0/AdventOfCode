#include <glib.h>

static GQueue *p1_deck = NULL;
static GQueue *p2_deck = NULL;

void aoc2020_day22_format_data(const gchar **lines, guint n_lines) {

    p1_deck = g_queue_new();
    p2_deck = g_queue_new();

    gboolean p2_i = 0;

    for (guint i=1; i<n_lines; i++) {
        if (!g_strcmp0("", lines[i])) {
            p2_i = i+2;
            break;
        }
        guint value = strtoul(lines[i], NULL, 10);
        g_queue_push_tail(p1_deck, GUINT_TO_POINTER(value));
    }

    for (guint i=p2_i; i<n_lines; i++) {
        guint value = strtoul(lines[i], NULL, 10);
        g_queue_push_tail(p2_deck, GUINT_TO_POINTER(value));
    }

}

static
gboolean round() {

    if (p1_deck->length == 0 || p2_deck->length == 0) {
        return FALSE;
    }

    guint p1_card = GPOINTER_TO_UINT(g_queue_pop_head(p1_deck));
    guint p2_card = GPOINTER_TO_UINT(g_queue_pop_head(p2_deck));

    if (p1_card > p2_card) {
        g_queue_push_tail(p1_deck, GUINT_TO_POINTER(p1_card));
        g_queue_push_tail(p1_deck, GUINT_TO_POINTER(p2_card));
    } else {
        g_queue_push_tail(p2_deck, GUINT_TO_POINTER(p2_card));
        g_queue_push_tail(p2_deck, GUINT_TO_POINTER(p1_card));
    }

    return TRUE;
}

void aoc2020_day22_p1(guint n_run) {

    while (round()) {  }

    GQueue *winner = (p1_deck->length == 0 ? p2_deck : p1_deck);

    guint64 score = 0;
    guint multiplier = 1;

    for (gint i=winner->length-1; i>=0; i--) {
        guint card = GPOINTER_TO_UINT(g_queue_peek_nth(winner, i));
        score += (card * multiplier);
        multiplier++;
    }

    if (!n_run) g_message("Part I: %llu", score);

}

static
gboolean recursive_combat(GQueue *p1, GQueue *p2) {

    if (p1->length == 0 || p2->length == 0) {
        return FALSE;
    }

    guint p1_card = GPOINTER_TO_UINT(g_queue_pop_head(p1));
    guint p2_card = GPOINTER_TO_UINT(g_queue_pop_head(p2));

    if (p1_card <= p1->length &&
        p2_card <= p2->length) {

        g_autoptr(GQueue) sub_p1 = g_queue_new();
        g_autoptr(GQueue) sub_p2 = g_queue_new();

        for (guint i=0; i<p1_card; i++) {
            g_queue_push_tail(sub_p1, GUINT_TO_POINTER(g_queue_peek_nth(p1, i)));
        }

        for (guint i=0; i<p2_card; i++) {
            g_queue_push_tail(sub_p2, GUINT_TO_POINTER(g_queue_peek_nth(p2, i)));
        }

        guint round = 0;
        while (recursive_combat(sub_p1, sub_p2)) {
            round++;
            if (round > 1000) {
                goto win_p1;
            }
        }

        if (sub_p1->length == 0) {
            goto win_p2;
        } else {
            goto win_p1;
        }


    } else if (p1_card > p2_card) {
        win_p1:;
        g_queue_push_tail(p1, GUINT_TO_POINTER(p1_card));
        g_queue_push_tail(p1, GUINT_TO_POINTER(p2_card));

    } else {
        win_p2:;
        g_queue_push_tail(p2, GUINT_TO_POINTER(p2_card));
        g_queue_push_tail(p2, GUINT_TO_POINTER(p1_card));

    }

    return TRUE;
}

void aoc2020_day22_p2(guint n_run) {

    while (recursive_combat(p1_deck, p2_deck)) { }

    GQueue *winner = (p1_deck->length == 0 ? p2_deck : p1_deck);

    guint64 score = 0;
    guint multiplier = 1;

    for (gint i=winner->length-1; i>=0; i--) {
        guint card = GPOINTER_TO_UINT(g_queue_peek_nth(winner, i));
        score += (card * multiplier);
        multiplier++;
    }

    if (!n_run) g_message("Part II: %llu", score);

}

void aoc2020_day22_cleanup() {

    g_queue_free(p1_deck);
    g_queue_free(p2_deck);

}
