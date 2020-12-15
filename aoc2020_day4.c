#include <glib.h>

static GPtrArray *passports = NULL;

#define NEW_PASSPORT g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free)
#define STREQ(L, R) (!g_strcmp0(L, R))

void aoc2020_day4_format_data(const gchar **lines, guint n_lines) {
    passports = g_ptr_array_new_full(0, (GDestroyNotify)g_hash_table_unref);
    GHashTable *p = NEW_PASSPORT;
    for (guint i=0; i<n_lines; i++) {
        if (STREQ(lines[i], "") || i==n_lines-1) { // Push new passport
            g_ptr_array_add(passports, p);
            p = i==n_lines-1 ? NULL : NEW_PASSPORT;
        } else { // Parse passport
            gchar **tokens = g_strsplit(lines[i], " ", 0);
            for (guint t=0; t<g_strv_length(tokens); t++) {
                gchar **keyval = g_strsplit(tokens[t], ":", 0);
                g_assert(g_strv_length(keyval) == 2);
                gchar *cleaned_val = g_strchomp(g_strdup(*(keyval+1)));
                g_hash_table_insert(p, g_strdup(*keyval), cleaned_val);
                g_strfreev(keyval);
            }
            g_strfreev(tokens);
        }
    }
}

#define HAS_KEY(K) (g_hash_table_contains(p, K))

void aoc2020_day4_p1(guint n_run) {

    guint valid = 0;
    for (guint i=0; i<passports->len; i++) {
        GHashTable *p = g_ptr_array_index(passports, i);
        if (HAS_KEY("byr") &&
            HAS_KEY("iyr") &&
            HAS_KEY("eyr") &&
            HAS_KEY("hgt") &&
            HAS_KEY("hcl") &&
            HAS_KEY("ecl") &&
            HAS_KEY("pid")) {
            valid += 1;
        }
    }
    if (!n_run) g_message("Part I: %d", valid);

}

#define RE_DECL(NAME, PATTERN) g_autoptr(GRegex) NAME##_re = g_regex_new(PATTERN, G_REGEX_OPTIMIZE, 0, NULL)
#define MATCHES(RE, STR) (g_regex_match(RE##_re, STR, 0, NULL))
#define VALUE(KEY) ((const gchar*)g_hash_table_lookup(p, KEY))

#include <stdio.h>

void aoc2020_day4_p2(guint n_run) {

    RE_DECL(byr, "(19[2-9][0-9]|200[0-2])");
    RE_DECL(iyr, "(20(1[0-9]|20))");
    RE_DECL(eyr, "(20(2[0-9]|30))");
    RE_DECL(hgt, "(1([5-8][0-9]|9[0-3])cm)|((59|6[0-9]|7[0-6])in)");
    RE_DECL(hcl, "^#[0-9a-f]{6}$");
    RE_DECL(ecl, "(amb|blu|brn|gry|grn|hzl|oth)");
    RE_DECL(pid, "^[0-9]{9}$");

    guint valid = 0;
    for (guint i=0; i<passports->len; i++) {
        GHashTable *p = g_ptr_array_index(passports, i);
        if (HAS_KEY("byr") && MATCHES(byr, VALUE("byr")) &&
            HAS_KEY("iyr") && MATCHES(iyr, VALUE("iyr")) &&
            HAS_KEY("eyr") && MATCHES(eyr, VALUE("eyr")) &&
            HAS_KEY("hgt") && MATCHES(hgt, VALUE("hgt")) &&
            HAS_KEY("hcl") && MATCHES(hcl, VALUE("hcl")) &&
            HAS_KEY("ecl") && MATCHES(ecl, VALUE("ecl")) &&
            HAS_KEY("pid") && MATCHES(pid, VALUE("pid"))) {
            valid += 1;
        }
    }

    if (!n_run) g_message("Part II: %d", valid);

}

void aoc2020_day4_cleanup() {
    g_ptr_array_unref(passports);
}
