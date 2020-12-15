#include <glib.h>

extern gint cmp_guint(gconstpointer a, gconstpointer b);

static const gchar **seats = NULL;
static guint n_seats = 0;

void aoc2020_day5_format_data(const gchar **lines, guint n_lines) {
    seats = lines;
    n_seats = n_lines;
}

static guint bp(const gchar* input, guint left, guint right, gchar left_char) {
    if (left == right) { return left; }
    guint distance = right - left;
    if (*input == left_char) {
        return bp(input+1, left, right - 1 - distance / 2, left_char);
    } else {
        return bp(input+1, left + 1 + distance / 2, right, left_char);
    }
}

#define ROW(STR) bp(STR, 0, 127, 'F')
#define COLUMN(STR) bp(STR, 0, 7, 'L')
#define SEAT_ID(STR) ((ROW(STR) * 8) + COLUMN(STR+7))

void aoc2020_day5_p1(guint n_run) {

    guint highest_seat_id = 0;

    for (guint i=0; i<n_seats; i++) {
        const gchar *seat = *(seats+i);
        highest_seat_id = MAX(highest_seat_id, SEAT_ID(seat));
    }

    if (!n_run) g_message("Part I: %d", highest_seat_id);

}

#define CONTAINS(A, ID) (g_array_binary_search(A, &ID, cmp_guint, NULL))

void aoc2020_day5_p2(guint n_run) {

    guint min_seat_id = UINT_MAX;
    guint max_seat_id = 0;
    g_autoptr(GArray) ids = g_array_sized_new(FALSE, FALSE, sizeof(guint), n_seats);

    for (guint i=0; i<n_seats; i++) {
        guint id = SEAT_ID(*(seats+i));
        max_seat_id = MAX(max_seat_id, id);
        min_seat_id = MIN(min_seat_id, id);
        g_array_append_val(ids, id);
    }

    g_array_sort(ids, cmp_guint);

    guint my_seat_id = 0;
    for (guint id=min_seat_id; id<max_seat_id; id++) {
        guint previous = id-1;
        guint next = id+1;
        if (!CONTAINS(ids, id) && CONTAINS(ids, previous) && CONTAINS(ids, next)) {
            my_seat_id = id;
            break;
        }
    }

    if (!n_run) g_message("Part II: %d.", my_seat_id);

}

void aoc2020_day5_cleanup() { }
