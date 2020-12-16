#include <glib.h>

// Range

typedef struct {
    guint left;
    guint right;
} range_t;

#define IN_RANGE(X, RANGE) (X >= RANGE.left && X <= RANGE.right)

// Rule

typedef struct {
    gchar *name;
    range_t first_range;
    range_t second_range;
    GArray *possible_indexes;
} rule_t;

#define IS_VALID(X, RULE) (IN_RANGE(X, RULE->first_range) || IN_RANGE(X, RULE->second_range))

static gpointer rule_copy(gconstpointer _r, gpointer user_data) {
    const rule_t *r = _r;
    rule_t *copy = g_malloc0(sizeof(*copy));
    copy->name = g_strdup(r->name);
    memcpy(&copy->first_range, &r->first_range, sizeof(range_t));
    memcpy(&copy->second_range, &r->second_range, sizeof(range_t));
    copy->possible_indexes = g_array_copy(r->possible_indexes);
    return copy;
}

static void rule_free(gpointer ptr) {
    if (ptr) {
        rule_t *r = ptr;
        g_free(r->name);
        g_array_unref(r->possible_indexes);
        g_free(r);
    }
}

static gboolean rule_matches_column(const rule_t *rule, const GArray *column) {
    for (guint c=0; c<column->len; c++) {
        guint value = g_array_index(column, guint, c);
        if (!IS_VALID(value, rule)) { return FALSE; }
    }
    return TRUE;
}

// Ticket

typedef struct {
    GArray *fields;
} ticket_t;

ticket_t *ticket_new() {
    ticket_t *t = g_malloc0(sizeof(*t));
    t->fields = g_array_new(FALSE, FALSE, sizeof(guint));
    return t;
}

gpointer ticket_copy(gconstpointer _t, gpointer user_data) {
    const ticket_t *t = _t;
    ticket_t *copy = g_malloc0(sizeof(*copy));
    copy->fields = g_array_copy(t->fields);
    return copy;
}

void ticket_free(gpointer ptr) {
    if (ptr) {
        ticket_t *t = ptr;
        g_array_unref(t->fields);
        g_free(t);
    }
}

// Parsing state

typedef enum {
    FORMAT_STATE_RULES = 0,
    FORMAT_STATE_MY_TICKET,
    FORMAT_STATE_NEARBY_TICKETS
} format_state_t;

// Data

static GPtrArray *rules = NULL;
static ticket_t *my_ticket = NULL;
static GPtrArray *nearby_tickets = NULL;

// Populated after part I
static GPtrArray *valid_nearby_tickets = NULL;
// Populated after parsing "my ticket"
static guint n_fields = 0;

void aoc2020_day16_format_data(const gchar **lines, guint n_lines) {

    gboolean should_skip_line = FALSE;
    rules = g_ptr_array_new_full(0, rule_free);
    nearby_tickets = g_ptr_array_new_full(0, ticket_free);
    valid_nearby_tickets = g_ptr_array_new_full(0, ticket_free);
    format_state_t state = FORMAT_STATE_RULES;

    for (guint i=0; i<n_lines; i++) {
        if (should_skip_line) {
            should_skip_line = FALSE;
            continue;
        }
        if (!g_strcmp0(*(lines+i), "")) {
            state += 1;
            should_skip_line = TRUE;
            if (state > FORMAT_STATE_NEARBY_TICKETS) {
                break;
            } else {
                continue;
            }
        }
        switch (state) {
            case FORMAT_STATE_RULES:
                {
                    // class: 1-3 or 5-7
                    gchar **tokens = g_strsplit(*(lines+i), ": ", 0);
                    g_assert(g_strv_length(tokens) == 2);
                    gchar **range = g_strsplit(tokens[1], " or ", 0);

                    gchar *name = g_strdup(*tokens);
                    range_t first_range = { strtoul(range[0], NULL, 10),
                                            strtoul(range[0]+strcspn(range[0], "-")+1, NULL, 10) };
                    range_t second_range = { strtoul(range[1], NULL, 10),
                                            strtoul(range[1]+strcspn(range[1], "-")+1, NULL, 10) };

                    rule_t *r = g_malloc0(sizeof(*r));
                    r->name = name;
                    memcpy(&r->first_range, &first_range, sizeof(first_range));
                    memcpy(&r->second_range, &second_range, sizeof(second_range));
                    r->possible_indexes = g_array_new(FALSE, FALSE, sizeof(guint));

                    g_ptr_array_add(rules, r);

                    g_strfreev(tokens);
                    g_strfreev(range);
                }
                break;
            case FORMAT_STATE_MY_TICKET:
                {
                    //1,2,3,4,5,6
                    gchar **tokens = g_strsplit(*(lines+i), ",", 0);
                    n_fields = g_strv_length(tokens);
                    my_ticket = ticket_new();

                    for (guint t=0; t<n_fields; t++) {
                        guint number = strtoul(tokens[t], NULL, 10);
                        g_array_append_val(my_ticket->fields, number);
                    }

                    g_strfreev(tokens);
                }
                break;
            default: // FORMAT_STATE_NEARBY_TICKETS
                {
                    gchar **tokens = g_strsplit(*(lines+i), ",", 0);
                    ticket_t *ticket = ticket_new();

                    for (guint t=0; t<n_fields; t++) {
                        guint number = strtoul(tokens[t], NULL, 10);
                        g_array_append_val(ticket->fields, number);
                    }

                    g_ptr_array_add(nearby_tickets, ticket);

                    g_strfreev(tokens);
                }
                break;
        }
    }

}

void aoc2020_day16_p1(guint n_run) {

    guint sum = 0;

    for (guint i=0; i<nearby_tickets->len; i++) {
        ticket_t *ticket = g_ptr_array_index(nearby_tickets, i);
        gboolean good = TRUE;
        for (guint f=0; f<ticket->fields->len; f++) {
            guint field = g_array_index(ticket->fields, guint, f);
            for (guint r=0; r<rules->len; r++) {
                rule_t *rule = g_ptr_array_index(rules, r);
                if (IS_VALID(field, rule)) { break; }
                if (r == rules->len-1 && !IS_VALID(field, rule)) {
                    sum += field;
                    good = FALSE;
                }
            }
        }
        if (good) {
            g_ptr_array_add(valid_nearby_tickets, ticket_copy(ticket, NULL));
        }
    }

    if (!n_run) g_message("Part I: %d", sum);
}

static gint rule_cmp(gconstpointer a, gconstpointer b) {
    rule_t *_a = *((rule_t **)a);
    rule_t *_b = *((rule_t **)b);
    return _a->possible_indexes->len - _b->possible_indexes->len;
}

void aoc2020_day16_p2(guint n_run) {

    // Populates valid nearby tickets without logging messages
    aoc2020_day16_p1(1);

    // Build columns from tickets (GPtArray of GArray)
    g_autoptr(GPtrArray) columns = g_ptr_array_new_full(n_fields, (GDestroyNotify )g_array_unref);

    for (guint f=0; f<n_fields; f++) {
        GArray *column = g_array_sized_new(FALSE, FALSE, sizeof(guint), n_fields);
        for (guint t=0; t<valid_nearby_tickets->len; t++) {
            ticket_t *ticket = g_ptr_array_index(valid_nearby_tickets, t);
            guint value = g_array_index(ticket->fields, guint, f);
            g_array_append_val(column, value);
        }
        g_ptr_array_add(columns, column);
    }

    // Test rules against columns and gather possible indexes
   for (guint r=0; r<rules->len; r++) {
       rule_t* rule = g_ptr_array_index(rules, r);
       for (guint c=0; c<columns->len; c++) {
           GArray *column = g_ptr_array_index(columns, c);
           if (rule_matches_column(rule, column)) {
               g_array_append_val(rule->possible_indexes, c);
           }
       }
   }

   g_autoptr(GPtrArray) rules_copy = g_ptr_array_copy(rules, rule_copy, NULL);

   gchar *final_rules[n_fields];
   guint finalized = 0;

   while (finalized < n_fields) {
       // Order rules from most to least constrained
       g_ptr_array_sort(rules_copy, rule_cmp);
       // Grab most constrained rule and remove its index from the remaining rules
       rule_t *rule = g_ptr_array_index(rules_copy, 0);
       guint rule_index = g_array_index(rule->possible_indexes, guint, 0);
       for (guint r=1; r<rules_copy->len; r++) {
           rule_t *next_rule = g_ptr_array_index(rules_copy, r);
           for (guint i=0; i<next_rule->possible_indexes->len; i++) {
               guint next_rule_index = g_array_index(next_rule->possible_indexes, guint, i);
               if (next_rule_index == rule_index) {
                   g_array_remove_index(next_rule->possible_indexes, i);
                   break;
               }
           }
       }
       final_rules[rule_index] = g_strdup(rule->name);
       finalized += 1;
       g_ptr_array_remove_index(rules_copy, 0);
   }

   // Multiply all values from my ticket where column name begins with "departure"
   guint64 mul = 1;
   for (guint i=0; i<n_fields; i++) {
       if (strstr(final_rules[i], "departure") == final_rules[i]) {
           guint my_ticket_value = g_array_index(my_ticket->fields, guint, i);
           mul *= my_ticket_value;
       }
   }

   // Free mem
   for (guint i=0; i<n_fields; i++) { g_free(final_rules[i]); }

   if (!n_run) g_message("Part II: %lld", mul);
}

void aoc2020_day16_cleanup() {
    g_ptr_array_unref(rules);
    ticket_free(my_ticket);
    g_ptr_array_unref(nearby_tickets);
    g_ptr_array_unref(valid_nearby_tickets);
}
