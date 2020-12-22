#include <glib.h>

typedef struct {
    GPtrArray *ingredients;
    GPtrArray *allergens;
} food_t;

static food_t* food_new() {
    food_t *f = g_new0(food_t, 1);
    f->ingredients = g_ptr_array_new_with_free_func(g_free);
    f->allergens = g_ptr_array_new_with_free_func(g_free);
    return f;
}

static void food_free(gpointer ptr) {
    if (ptr) {
        food_t *f = ptr;
        g_ptr_array_unref(f->ingredients);
        g_ptr_array_unref(f->allergens);
        g_free(f);
    }
}

static GPtrArray *allergens = NULL;
static GPtrArray *foods = NULL;
static GHashTable *mapping = NULL;
static GPtrArray *poisons = NULL;

void aoc2020_day21_format_data(const gchar **lines, guint n_lines) {

    allergens = g_ptr_array_new_with_free_func(g_free);
    foods = g_ptr_array_new_with_free_func(food_free);

    g_autoptr(GRegex) re = g_regex_new(
            "(?<ingredients>(?: ?\\w+)+) \\(contains (?<allergens>(?:(?:, )?(?:\\w+))+)\\)",
            G_REGEX_OPTIMIZE,
            0,
            NULL);

    for (guint i=0; i<n_lines; i++) {

        GMatchInfo *info = NULL;

        if (g_regex_match(re, lines[i], 0, &info)) {

            food_t *food = food_new();

            g_autofree gchar *_ingredients = g_match_info_fetch_named(info, "ingredients");
            g_autofree gchar *_allergens = g_match_info_fetch_named(info, "allergens");

            gchar **_ingredients_tokens = g_strsplit(_ingredients, " ", 0);
            gchar **_allergens_tokens = g_strsplit(_allergens, ", ", 0);

            for (guint f=0; f<g_strv_length(_ingredients_tokens); f++) {
                g_ptr_array_add(food->ingredients, _ingredients_tokens[f]);
            }
            for (guint a=0; a<g_strv_length(_allergens_tokens); a++) {
                g_ptr_array_add(food->allergens, _allergens_tokens[a]);
                if (!g_ptr_array_find_with_equal_func(allergens,  _allergens_tokens[a], g_str_equal, NULL)) {
                    g_ptr_array_add(allergens, g_strdup(_allergens_tokens[a]));
                }
            }

            g_ptr_array_add(foods, food);

            g_free(_ingredients_tokens);
            g_free(_allergens_tokens);

        }
    }

}

static
gboolean food_has_allergen(food_t *food, gchar *allergen) {
    for (guint i=0; i<food->allergens->len; i++) {
        gchar *x = g_ptr_array_index(food->allergens, i);
        if (!g_strcmp0(x, allergen)) {
            return TRUE;
        }
    }
    return FALSE;
}

static
gpointer g_str_copy(gconstpointer src, gpointer data) {
    return g_strdup(src);
}

static
GPtrArray *g_ptr_array_intersection(GPtrArray *left,
                                    GPtrArray *right,
                                    GEqualFunc equal_func,
                                    GCopyFunc copy_func,
                                    GDestroyNotify destroy_func) {

    GPtrArray *result = g_ptr_array_new_with_free_func(destroy_func);
    for (guint l=0; l<left->len; l++) {
        gpointer lptr = g_ptr_array_index(left, l);
        for (guint r=0; r<right->len; r++) {
            gpointer rptr = g_ptr_array_index(right, r);
            if (equal_func(lptr, rptr)) {
                g_ptr_array_add(result, copy_func(lptr, NULL));
                goto next_l;
            }
        }
        next_l:;
    }
    return result;

}

static
gint g_str_sort(gconstpointer a, gconstpointer b)
{
    gchar *str_a = (gchar *)a;
    gchar *str_b = (gchar *)b;
    return g_strcmp0(str_a, str_b);
}

void aoc2020_day21_p1(guint n_run) {

    mapping = g_hash_table_new_full(g_str_hash,
                                    g_str_equal,
                                    g_free,
                                    (GDestroyNotify) g_ptr_array_unref);

    // Intersect all foods that contain the same allergen
    for (guint a=0; a<allergens->len; a++) {
        gchar *allergen = g_ptr_array_index(allergens, a);
        GPtrArray *tmp = g_ptr_array_new_with_free_func(g_free);
        for (guint x=0; x<foods->len; x++) {
            food_t *f = g_ptr_array_index(foods, x);
            if (food_has_allergen(f, allergen)) {
                if (tmp->len == 0) {
                    tmp = g_ptr_array_copy(f->ingredients, g_str_copy, NULL);
                } else {
                    GPtrArray *intersection =
                            g_ptr_array_intersection(tmp,
                                                     f->ingredients,
                                                     g_str_equal,
                                                     g_str_copy,
                                                     g_free);
                    g_ptr_array_unref(tmp);
                    tmp = intersection;
                }
            }
        }
        g_hash_table_insert(mapping, g_strdup(allergen), tmp);
    }

    // Simplify
    while TRUE {
        guint not_simplified = 0;
        for (guint a=0; a<allergens->len; a++) {
            gchar *allergen = g_ptr_array_index(allergens, a);
            GPtrArray *candidates = g_hash_table_lookup(mapping, allergen);

            if (candidates->len == 1) {

                gchar *candidate = g_ptr_array_index(candidates, 0);

                GHashTableIter it;
                g_hash_table_iter_init(&it, mapping);
                gpointer key, value;

                while (g_hash_table_iter_next(&it, &key, &value)) {
                    gchar *_allergen = key;
                    if (!g_strcmp0(_allergen, allergen)) continue;
                    GPtrArray *_candidates = value;
                    guint index=0;
                    if (g_ptr_array_find_with_equal_func(_candidates, candidate, g_str_equal, &index)) {
                        g_ptr_array_remove_index(_candidates, index);
                    }
                }

            } else {

                not_simplified++;

            }
        }
        if (!not_simplified) break;
    }

    // Count
    poisons = g_ptr_array_new_with_free_func(g_free);
    guint count=0;

    GHashTableIter it;
    g_hash_table_iter_init(&it, mapping);
    gpointer key, value;

    while (g_hash_table_iter_next(&it, &key, &value)) {
        gchar *poison = g_ptr_array_index(((GPtrArray*)value), 0);
        g_ptr_array_add(poisons, g_strdup(poison));
    }

    for (guint f=0; f<foods->len; f++) {
        food_t *food = g_ptr_array_index(foods, f);
        for (guint p=0; p<food->ingredients->len; p++) {
            gchar *ingredient = g_ptr_array_index(food->ingredients, p);
            if (!g_ptr_array_find_with_equal_func(poisons, ingredient, g_str_equal, NULL)) {
                count += 1;
            }
        }
    }

    if (!n_run) g_message("Part I: %d", count);

}

void aoc2020_day21_p2(guint n_run) {

    aoc2020_day21_p1(1); // to populate mapping

    GList *keys = g_hash_table_get_keys(mapping);
    keys = g_list_sort(keys, g_str_sort);

    g_autoptr(GString) list = g_string_new("");

    for (guint i=0; i<poisons->len; i++) {
        gchar *allergen = g_list_nth_data(keys, i);
        gchar *element = g_ptr_array_index((GPtrArray *)g_hash_table_lookup(mapping, allergen), 0);
        if (i == poisons->len - 1) {
            g_string_append_printf(list, "%s", element);
        } else {
            g_string_append_printf(list, "%s,", element);
        }
    }

    g_list_free(keys);

    if (!n_run) g_message("Part II: %s", list->str);

}

void aoc2020_day21_cleanup() {
    g_ptr_array_unref(foods);
    g_ptr_array_unref(allergens);
    g_ptr_array_unref(poisons);
}
