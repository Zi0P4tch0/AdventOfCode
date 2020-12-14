#include <glib.h>

#define ADDRESS_SPACE 36

typedef struct {
    gchar mask[ADDRESS_SPACE+1];
    guint64 address;
    guint64 value;
} instruction_t;

static GArray *instructions = NULL;

void aoc2020_day14_format_data(const gchar **lines, guint n_lines) {

    instructions = g_array_new(FALSE, FALSE, sizeof(instruction_t));

    gchar mask[ADDRESS_SPACE+1];

    for (guint i=0; i<n_lines; i++) {

        if (strstr(*(lines+i), "mask") != NULL) { // Update mask

            gchar **tokens = g_strsplit(*(lines+i), " = ", 0);

            memcpy(mask, tokens[1], ADDRESS_SPACE);
            mask[ADDRESS_SPACE] = '\0';

            g_strfreev(tokens);

        } else { // Generate instructions

            gchar **tokens = g_strsplit(*(lines+i), " = ", 0);

            instruction_t ins;
            memcpy(&ins.mask, mask, ADDRESS_SPACE+1);
            ins.address = strtoull(tokens[0]+4, NULL, 10);
            ins.value = strtoull(tokens[1], NULL, 10);

            g_array_append_val(instructions, ins);

            g_strfreev(tokens);

        }
    }
}

static guint g_uint64_hash(gconstpointer ptr) {
    return (guint) *((const guint64 *)ptr);
}

static gboolean g_uint64_equal(gconstpointer v1, gconstpointer v2) {
    return *((const guint64*) v1) == *((const guint64*) v2);
}

static void sum(gpointer key, gpointer value, gpointer user_data) {
    const guint64* v = (const guint64*)value;
    guint64 *total = (guint64 *)user_data;
    *total += *v;
}

void aoc2020_day14_p1(guint n_run) {

    g_autoptr(GHashTable) memory = g_hash_table_new_full(g_uint64_hash, g_uint64_equal, g_free, g_free);

    for (guint i = 0; i < instructions->len; i++) {

        instruction_t instruction = g_array_index(instructions, instruction_t, i);

        // Generate clear / set masks
        guint64 clear = 0, set = 0;
        for (gint m=0; m<ADDRESS_SPACE; m++) {
            set <<= 1;
            clear <<=1;
            if (instruction.mask[m] == '1') {
                set |= 1;
                clear |= 1;
            }
            if (instruction.mask[m] == 'X') {
                clear |= 1;
            }
        }

        guint64 *key = g_malloc(sizeof(guint64));
        *key = instruction.address;
        guint64 *value = g_malloc(sizeof(guint64));
        *value = (instruction.value & clear) | set;

        g_hash_table_insert(memory, key, value);

    }

    guint64 total_sum = 0;
    g_hash_table_foreach(memory, sum, &total_sum);

    if (!n_run) g_message("Part I: %lld", total_sum);

}

static void mask_combinations(const gchar *template, GPtrArray **tmp) {
    g_assert(tmp);

    if (!*tmp) {
        guint n_x = 0;
        for (guint i=0; i<strlen(template); i++) { if (template[i] == 'X') n_x += 1; }
        guint n_combo = 1UL << n_x;
        *tmp = g_ptr_array_new_full(n_combo, g_free);
    }

    gchar *x_location = NULL;

    if ((x_location = strstr(template, "X")) == NULL) {
        g_ptr_array_add(*tmp, g_strdup(template));
    } else {
        gchar m1[ADDRESS_SPACE+1], m2[ADDRESS_SPACE+1];
        memcpy(m1, template, ADDRESS_SPACE+1);
        memcpy(m2, template, ADDRESS_SPACE+1);
        m1[x_location - template] = '0';
        m2[x_location - template] = '1';
        mask_combinations(m1, tmp);
        mask_combinations(m2, tmp);
    }
    
}

// https://stackoverflow.com/a/3208376/2890168
// Behold, preprocessor poetry!

#define UINT36_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define UINT36_TO_BINARY(N)         \
  (N & 0x800000000 ? '1' : '0'),    \
  (N & 0x400000000 ? '1' : '0'),    \
  (N & 0x200000000 ? '1' : '0'),    \
  (N & 0x100000000 ? '1' : '0'),    \
  (N & 0x80000000 ? '1' : '0'),     \
  (N & 0x40000000 ? '1' : '0'),     \
  (N & 0x20000000 ? '1' : '0'),     \
  (N & 0x10000000 ? '1' : '0'),     \
  (N & 0x8000000 ? '1' : '0'),      \
  (N & 0x4000000 ? '1' : '0'),      \
  (N & 0x2000000 ? '1' : '0'),      \
  (N & 0x1000000 ? '1' : '0'),      \
  (N & 0x800000 ? '1' : '0'),       \
  (N & 0x400000 ? '1' : '0'),       \
  (N & 0x200000 ? '1' : '0'),       \
  (N & 0x100000 ? '1' : '0'),       \
  (N & 0x80000 ? '1' : '0'),        \
  (N & 0x40000 ? '1' : '0'),        \
  (N & 0x20000 ? '1' : '0'),        \
  (N & 0x10000 ? '1' : '0'),        \
  (N & 0x8000 ? '1' : '0'),         \
  (N & 0x4000 ? '1' : '0'),         \
  (N & 0x2000 ? '1' : '0'),         \
  (N & 0x1000 ? '1' : '0'),         \
  (N & 0x800 ? '1' : '0'),          \
  (N & 0x400 ? '1' : '0'),          \
  (N & 0x200 ? '1' : '0'),          \
  (N & 0x100 ? '1' : '0'),          \
  (N & 0x80 ? '1' : '0'),           \
  (N & 0x40 ? '1' : '0'),           \
  (N & 0x20 ? '1' : '0'),           \
  (N & 0x10 ? '1' : '0'),           \
  (N & 0x08 ? '1' : '0'),           \
  (N & 0x04 ? '1' : '0'),           \
  (N & 0x02 ? '1' : '0'),           \
  (N & 0x01 ? '1' : '0')

void aoc2020_day14_p2(guint n_run) {

    g_autoptr(GHashTable) memory = g_hash_table_new_full(g_uint64_hash, g_uint64_equal, g_free, g_free);

    for (guint i=0; i<instructions->len; i++) {

        instruction_t instruction = g_array_index(instructions, instruction_t, i);

        g_autofree gchar *binary_address =g_strdup_printf(
                UINT36_TO_BINARY_PATTERN,
                UINT36_TO_BINARY(instruction.address));

        const gchar *mask = instruction.mask;

        for (guint m=0; m<ADDRESS_SPACE; m++) {
            if (mask[m] == 'X') {
                binary_address[m] = 'X';
            }
            if (mask[m] == '1') {
                binary_address[m] = '1';
            }
        }

        g_autoptr(GPtrArray) addresses = NULL;
        mask_combinations(binary_address, &addresses);

        for (guint c=0; c<addresses->len; c++) {

            const gchar *c_address = g_ptr_array_index(addresses, c);
            guint64 c_address_n = strtol(c_address, NULL, 2);

            guint64 *key = g_malloc(sizeof(guint64));
            *key = c_address_n;
            guint64 *value = g_malloc(sizeof(guint64));
            *value = instruction.value;

            g_hash_table_insert(memory, key, value);
        }

    }

    guint64 total_sum = 0;
    g_hash_table_foreach(memory, &sum, &total_sum);

    if (!n_run) g_message("Part II: %lld", total_sum);
}

void aoc2020_day14_cleanup() {
    g_array_unref(instructions);
}
