#include <glib.h>

static gint timestamp = 0;
static GArray *buses = NULL;

#define WILDCARD_BUS_ID (gint)0

void aoc2020_day13_format_data(const gchar **lines, guint n_lines) {
    g_assert(n_lines == 2);
    timestamp = strtol(*lines, NULL, 10);
    gchar **_ids = g_strsplit(lines[1], ",", 0);
    buses = g_array_new(FALSE, FALSE, sizeof(gint));
    for (guint i=0; i<g_strv_length(_ids); i++) {
        gint value = 0;
        if (!g_strcmp0("x", _ids[i])) {
            value = WILDCARD_BUS_ID;
        } else {
            value = strtol(_ids[i], NULL, 10);
        }
        g_array_append_val(buses, value);
    }
}

void aoc2020_day13_p1(guint n_run) {
    gint target_bus = 0, target_wait_time = INT_MAX;
    for (guint i=0; i<buses->len; i++) {
        gint bus = g_array_index(buses, gint, i);
        if (bus == WILDCARD_BUS_ID) { continue; }
        gint wait_time = bus - (timestamp % bus);
        if (wait_time < target_wait_time) {
            target_bus = bus;
            target_wait_time = wait_time;
        }
    }
    if (!n_run) g_message("Part I: %d.", target_bus * target_wait_time);
}

void aoc2020_day13_p2(guint n_run) {
    unsigned long long t = 0, increment = g_array_index(buses, gint, 0);
    for (guint i=1; i<buses->len; i++) {
        gint bus = g_array_index(buses, gint, i);
        if (bus == WILDCARD_BUS_ID) continue;
        while TRUE {
            t += increment;
            if ((t + i) % bus == 0) {
                increment *= bus;
                break;
            }
        }
    }
    if (!n_run) g_message("Part II: %llu.", t);
}

void aoc2020_day13_cleanup() {
    g_array_unref(buses);
}
