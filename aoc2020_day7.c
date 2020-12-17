#include <glib.h>

static GHashTable *contains = NULL;


void aoc2020_day7_format_data(const gchar **lines, guint n_lines) {

    g_autoptr(GRegex) color_re = g_regex_new("(?<color>.+?) bags",
                                             G_REGEX_OPTIMIZE,
                                             0,
                                             NULL);
    g_autoptr(GRegex) contains_re = g_regex_new("(?<quantity>\\d+) (?<color>.+?) bag",
                                                G_REGEX_OPTIMIZE,
                                                0,
                                                NULL);

    contains = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify) g_variant_unref);

    for (guint i=0; i<n_lines; i++) {

        g_autoptr(GMatchInfo) color_match_info;
        g_regex_match(color_re, *(lines+i), 0, &color_match_info);
        gchar *color = g_match_info_fetch_named(color_match_info, "color");

        g_autoptr(GMatchInfo) contains_match_info;
        g_regex_match(contains_re, *(lines+i), 0, &contains_match_info);
        gint count = g_match_info_get_match_count(contains_match_info);

        if (count == 0) {

            g_autoptr(GVariantBuilder) builder = g_variant_builder_new(G_VARIANT_TYPE("a(us)"));
            GVariant *value = g_variant_builder_end(builder);

            g_hash_table_insert(contains, color, g_variant_ref_sink(value));

        } else {

            g_autoptr(GVariantBuilder) builder = g_variant_builder_new(G_VARIANT_TYPE("a(us)"));

            do  {

                g_autofree gchar *q = g_match_info_fetch_named(contains_match_info, "quantity");
                g_autofree gchar *c = g_match_info_fetch_named(contains_match_info, "color");

                g_variant_builder_add(builder, "(us)", strtoul(q, NULL, 10), c);

            } while (g_match_info_next(contains_match_info, NULL));

            GVariant *value = g_variant_builder_end(builder);

            g_hash_table_insert(contains,
                                color,
                                g_variant_ref_sink(value));
        }
    }

}


static gboolean contains_shiny(gchar *bag) {
    gboolean value = FALSE;
    if (!g_strcmp0("shiny gold", bag)) { return TRUE; }
    const GVariant *v = g_hash_table_lookup(contains, bag);
    GVariantIter it;
    g_variant_iter_init(&it, (GVariant *)v);
    gchar *color;
    while (g_variant_iter_next(&it, "(us)", NULL, &color)) {
        value = value || contains_shiny(color);
    }
    return value;
}

static guint count_bag(gchar *bag) {
    guint count = 1;
    const GVariant *v = g_hash_table_lookup(contains, bag);
    GVariantIter it;
    g_variant_iter_init(&it, (GVariant *)v);
    guint quantity;
    gchar *color;
    while (g_variant_iter_next(&it, "(us)", &quantity, &color)) {
        count += (quantity * count_bag(color));
    }
    return count;
}

void aoc2020_day7_p1(guint n_run) {

    guint n_keys = 0, count = 0;
    const gchar **keys = (const gchar **) g_hash_table_get_keys_as_array(contains, &n_keys);
    for (guint k=0; k<n_keys; k++) {
        const gchar *key = keys[k];
        count += contains_shiny((gchar*)key) ? 1 : 0;
    }

    if (!n_run) g_message("Part I: %d", count-1);

}

void aoc2020_day7_p2(guint n_run) {

    if (!n_run) g_message("Part II: %d", count_bag("shiny gold")-1);

}

void aoc2020_day7_cleanup() {
    g_hash_table_unref(contains);
}
