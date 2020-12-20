#include <glib.h>

typedef enum {
    RULE_TYPE_SINGLE_CHAR,
    RULE_TYPE_OR,
    RULE_TYPE_AND
} rule_type_t;

typedef struct {
    rule_type_t type;
    gint id;
} rule_t;

typedef struct {
    rule_type_t type;
    gint id;
    gchar c;
} rule_char_t;

typedef struct {
    rule_type_t type;
    gint id;
    GArray *lhs;
    GArray *rhs;
} rule_or_t;

typedef struct {
    rule_type_t type;
    gint id;
    GArray *rules;
} rule_and_t;

static
rule_t* rule_char_new(gchar c) {
    rule_char_t *r = g_new0(rule_char_t, 1);
    r->type = RULE_TYPE_SINGLE_CHAR;
    r->c = c;
    return (rule_t*)r;
}

static
rule_t* rule_or_new(GArray *lhs, GArray *rhs) {
    rule_or_t *r = g_new0(rule_or_t, 1);
    r->type = RULE_TYPE_OR;
    r->lhs = lhs;
    r->rhs = rhs;
    return (rule_t*)r;
}

static
rule_t* rule_and_new(GArray *rules) {
    rule_and_t *r = g_new0(rule_and_t, 1);
    r->type = RULE_TYPE_AND;
    r->rules = rules;
    return (rule_t*)r;
}

static
void rule_free(gpointer p) {
    if (p) {
        rule_t *r = p;
        if (r->type == RULE_TYPE_OR) {
            g_array_unref(((rule_or_t *)r)->lhs);
            g_array_unref(((rule_or_t *)r)->rhs);
        } else if (r->type == RULE_TYPE_AND) {
            g_array_unref(((rule_and_t *)r)->rules);
        }
        g_free(r);
    }
}

static GHashTable *rules = NULL;
static gboolean is_part_2 = FALSE;
static gchar *part_2_eight = NULL;
static gchar *part_2_eleven = NULL;
static GPtrArray *messages = NULL;

void aoc2020_day19_format_data(const gchar **lines, guint n_lines) {

    g_autoptr(GRegex) rule_re = g_regex_new("(?<id>\\d+): (?:(?:\\\"(?<char>[a-z])\\\")|(?<lhs>\\d+(?: \\d+)*)(?: \\| (?<rhs>\\d+(?: \\d+)*))?)",
                                            G_REGEX_OPTIMIZE,
                                            0,
                                            NULL);

    rules = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, rule_free);

    guint separator_index = -1;

    // Rules
    for (guint i=0; i<n_lines; i++) {

        if (!g_strcmp0(lines[i], "")) {
            separator_index = i;
            break;
        }

        g_autoptr(GMatchInfo) match_info;
        if (g_regex_match(rule_re, lines[i], 0, &match_info)) {

            g_autofree gchar *id = g_match_info_fetch_named(match_info, "id");
            g_autofree gchar *c = g_match_info_fetch_named(match_info, "char");
            g_autofree gchar *lhs = g_match_info_fetch_named(match_info, "lhs");
            g_autofree gchar *rhs = g_match_info_fetch_named(match_info, "rhs");

            gint *key = g_malloc(sizeof(*key));
            *key = strtol(id, NULL, 10);

            rule_t *value;

            if (c != NULL && g_strcmp0(c, "")) { // char type

                value = rule_char_new(*c);

            } else if (rhs == NULL) { // and

                gchar **tokens = g_strsplit(lhs, " ", 0);
                GArray *r = g_array_new(FALSE, FALSE, sizeof(gint));
                for (guint t=0; t<g_strv_length(tokens); t++) {
                    gint v = strtol(tokens[t], NULL, 10);
                    g_array_append_val(r, v);
                }

                value = rule_and_new(r);

                g_strfreev(tokens);

            } else { // or

                gchar **lhs_tokens = g_strsplit(lhs, " ", 0);
                gchar **rhs_tokens = g_strsplit(rhs, " ", 0);

                GArray *l = g_array_new(FALSE, FALSE, sizeof(gint));
                for (guint t=0; t<g_strv_length(lhs_tokens); t++) {
                    gint v = strtol(lhs_tokens[t], NULL, 10);
                    g_array_append_val(l, v);
                }

                GArray *r = g_array_new(FALSE, FALSE, sizeof(gint));
                for (guint t=0; t<g_strv_length(rhs_tokens); t++) {
                    gint v = strtol(rhs_tokens[t], NULL, 10);
                    g_array_append_val(r, v);
                }

                value = rule_or_new(l, r);

                g_strfreev(lhs_tokens);
                g_strfreev(rhs_tokens);
            }

            value->id = *key;

            g_hash_table_insert(rules, key, value);

        }
    }

    // Messages

    messages = g_ptr_array_new_full(0, g_free);

    for (guint i=separator_index+1; i<n_lines; i++) {
        g_ptr_array_add(messages, g_strdup(lines[i]));
    }

}

#define R_CHAR(R) ((rule_char_t*)R)
#define R_AND(R) ((rule_and_t*)R)
#define R_OR(R) ((rule_or_t*)R)

static
gchar *rule_regex_string(rule_t *r) {
    if (is_part_2 && r->id == 8) {
        return g_strdup(part_2_eight);
    }
    if (is_part_2 && r->id == 11) {
        return g_strdup(part_2_eleven);
    }
    if (r->type == RULE_TYPE_SINGLE_CHAR) {
        return g_strdup_printf("%c", R_CHAR(r)->c);
    } else if (r->type == RULE_TYPE_AND) {
        g_autoptr(GString) s = g_string_new("");
        for (guint i=0; i<R_AND(r)->rules->len; i++) {
            gint subrule_key = g_array_index(R_AND(r)->rules, gint, i);
            rule_t *subrule = g_hash_table_lookup(rules, &subrule_key);
            g_autofree gchar *subregex = rule_regex_string(subrule);
            g_string_append(s, subregex);
        }
        return g_strdup(s->str);
    } else { // OR
        g_autoptr(GString) s = g_string_new("(?:");
        for (guint i=0; i<R_OR(r)->lhs->len; i++) {
            gint subrule_key = g_array_index(R_OR(r)->lhs, gint, i);
            rule_t *subrule = g_hash_table_lookup(rules, &subrule_key);
            g_autofree gchar *subregex = rule_regex_string(subrule);
            g_string_append(s, subregex);
        }
        g_string_append(s, "|");
        for (guint i=0; i<R_OR(r)->rhs->len; i++) {
            gint subrule_key = g_array_index(R_OR(r)->rhs, gint, i);
            rule_t *subrule = g_hash_table_lookup(rules, &subrule_key);
            g_autofree gchar *subregex = rule_regex_string(subrule);
            g_string_append(s, subregex);
        }
        g_string_append(s, ")");
        return g_strdup(s->str);
    }
}

static
GRegex *rule_regex(rule_t *r) {
    g_autofree gchar *regex = rule_regex_string(r);
    g_autofree gchar *proper_regex = g_strdup_printf("^%s$", regex);
    return g_regex_new(proper_regex,
                       G_REGEX_OPTIMIZE,
                       0,
                       NULL);
}

void aoc2020_day19_p1(guint n_run) {

    guint key = 0;
    rule_t *rule_zero = g_hash_table_lookup(rules, &key);

    g_autoptr(GRegex) re = rule_regex(rule_zero);

    guint matches = 0;

    for(guint i=0; i<messages->len; i++) {
        matches += g_regex_match(re,
                                 g_ptr_array_index(messages, i),
                                 0,
                                 NULL);
    }

    if (!n_run) g_message("Part I: %d", matches);
}

void aoc2020_day19_p2(guint n_run) {

    is_part_2 = TRUE;

    gint key = 42;
    g_autofree gchar *_42 = rule_regex_string(g_hash_table_lookup(rules, &key));

    key = 31;
    g_autofree gchar *_31 = rule_regex_string(g_hash_table_lookup(rules, &key));

    // Modified 8 rule
    part_2_eight = g_strdup_printf("(?:(?:%s)+)", _42);

    // Modified 11 rule
    part_2_eleven = g_strdup_printf("(?<eleven>(?:%s)(?&eleven)?(?:%s))", _42, _31);

    key = 0;
    rule_t *rule_zero = g_hash_table_lookup(rules, &key);

    g_autoptr(GRegex) re = rule_regex(rule_zero);

    guint matches = 0;

    for(guint i=0; i<messages->len; i++) {
        matches += g_regex_match(re,
                                 g_ptr_array_index(messages, i),
                                 0,
                                 NULL);
    }

    if (!n_run) g_message("Part II: %d", matches);

}

void aoc2020_day19_cleanup() {
    g_hash_table_unref(rules);
    g_ptr_array_unref(messages);
    if (is_part_2) {
        g_free(part_2_eight);
        g_free(part_2_eleven);
    }
}
