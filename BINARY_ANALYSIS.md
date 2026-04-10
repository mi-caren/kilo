# Binary Analysis — Full vs Tiny Aeolus Variants

Compiled with `gcc-15 -O3 --std=c23` on 2026-04-10.
All builds use `-ffunction-sections -fdata-sections` + `-Wl,--gc-sections`.

## Round 1 — Before reorganization (single libaeolus.a)

Both full and tiny .c files were compiled into one `libaeolus.a`.
Four binaries: `{full,tiny} × {default, gc-sections}`.

| Binary | .text | .data | Total | Text Symbols |
|--------|------:|------:|------:|:------------:|
| full | 43,510 | 1,136 | 65K | 241 |
| full + gc-sections | 41,166 | 1,136 | 60K | 219 |
| tiny | 46,483 | 1,144 | 71K | 279 |
| tiny + gc-sections | **33,011** | 1,144 | **53K** | **157** |

### Aeolus library overhead (gc-sections builds)

| Component | full | tiny | Delta |
|-----------|-----:|-----:|------:|
| Vec* functions | 5,932 B | 824 B | **-86%** |
| TinyVec* functions | — | 1,044 B | — |
| Iterator* functions | 1,297 B | 592 B | **-54%** |
| str_* functions | 948 B | 948 B | **identical** |
| **Total aeolus** | **8,177 B** | **3,408 B** | **-58%** |

### Duplicate symbol problem

Both `string/string.c` and `tiny/string.c` were compiled into the same archive.
This caused the tiny binary to contain BOTH `Vecchar_*` (824 B, leaked from the
full string.c) AND `TinyVecchar_*` (1,044 B) — wasting ~824 B.

---

## Round 2 — After reorganization (separate libraries)

Changes applied:
- **Separate libraries**: `libaeolus.a` (full) and `libaeolus_tiny.a` (tiny)
- **Unified Vec prefix**: `Vec(TYPE)` in shared/vec.h — no more `TinyVec` prefix
- **Directory reorganization**: full headers at aeolus root, shared infra in shared/
- **Link-time variant guard**: sentinel symbol prevents header/library mismatch

| Binary | .text | .data | Total | Text Symbols |
|--------|------:|------:|------:|:------------:|
| full_02 (gc-sections) | 41,731 | 1,360 | 66K | 219 |
| **tiny_02** (gc-sections) | **32,013** | 1,368 | **54K** | **141** |

| Library | Size |
|---------|-----:|
| libaeolus.a | 46,284 B |
| libaeolus_tiny.a | 36,484 B |

Note: the total binary size of tiny_02 (53,552 B) is slightly larger than
tiny R1 (53,192 B) despite having 998 fewer bytes of .text. This is because
the link-time variant guards (`_aeolus_check_*` pointers with
`__attribute__((retain))`) add ~224 B to .data — each 8-byte pointer survives
gc-sections by design (~10 TUs × 3 headers). The .text improvement outweighs
this, but total file size also includes ELF headers and symbol table entries
for the sentinel symbols.

### Aeolus library overhead (round 2)

| Component | full_02 | tiny_02 | Delta |
|-----------|--------:|--------:|------:|
| Vec* functions | 5,932 B | 1,528 B | **-74%** |
| Iterator* functions | 1,297 B | 304 B | **-77%** |
| str_* functions | 1,019 B | 977 B | ~identical |
| **Total aeolus** | **8,248 B** | **2,809 B** | **-66%** |

### What improved from round 1 to round 2

| Metric | tiny R1 | tiny R2 | Improvement |
|--------|--------:|--------:|------------:|
| .text | 33,011 B | 32,013 B | **-3.0%** |
| Text symbols | 157 | 141 | **-10.2%** |
| Aeolus overhead | 3,408 B | 2,809 B | **-17.6%** |
| Duplicate Vec symbols | 824 B wasted | 0 | **eliminated** |
| TinyVec symbols | 1,044 B | 0 | **eliminated** (unified prefix) |

The separate library build eliminated the duplicate `Vecchar_*` + `TinyVecchar_*`
symbols. With the unified `Vec` prefix, the tiny binary now has exactly one set
of `Vecchar_*` functions — only the 18 that weren't fully inlined by the compiler.

The tiny_02 binary keeps only **18 Vec functions** and **8 Iterator functions**
(vs 78+ compiled). gc-sections strips everything the compiler inlined.

---

## Why full can't strip Vec functions

The full variant stores a `vec_driver` vtable (function pointer table) on each
Vec instance. The vtable references ALL Vec functions (push, pop, get, set,
insert, remove, ...) regardless of which ones the program actually calls. Since
every function is "referenced" by the vtable, `--gc-sections` cannot strip any
of them.

Full keeps **71 Vec functions** and **35 Iterator functions** even with gc-sections.

## Why tiny strips so effectively

Tiny dispatches via direct function calls (no vtable). At -O3, the compiler
inlines most of these calls. The original function bodies remain in the object
file (external linkage), but with `-ffunction-sections` each body goes into its
own ELF section. `-Wl,--gc-sections` then strips every section that nothing
references — i.e. every function body that was fully inlined at all call sites.

## String _fn macros: NOT counterproductive

The `str_*` functions use `_fn` macros (direct calls to `Vecchar_push`,
`Vecchar_pop`, etc.) even in the full variant. This is **not** counterproductive:

- **Same binary size**: str_ functions are ~950-1000 bytes in both variants.
  The _fn calls compile into the str_ function bodies themselves. Since str_
  functions have external linkage and aren't inlined at their call sites, the
  generated code is the same regardless of variant.
- **Better runtime performance**: _fn macros bypass the vtable indirection.
  Instead of `self->driver->push(self, el)` (indirect call through function
  pointer), the compiler sees `Vecchar_push(self, el)` (direct call) which it
  can inline at -O3. This eliminates branch prediction misses from indirect calls.
- **No extra code generated**: the _fn macros just expand to the function name.
  They don't create duplicate code.

**Conclusion**: using _fn in string is the right choice. It gives full-variant
string operations the same inlining opportunities as tiny, with zero cost.

## App function size comparison (gc-sections, round 2)

Some functions are **larger** in tiny because the compiler inlines vec operations
into the call site, expanding the function body:

| Function | full | tiny | Delta |
|----------|-----:|-----:|------:|
| editorRowRender | 592 B | 693 B | +17% |
| editorRowHighlightSyntax | 1,940 B | 1,988 B | +2.5% |
| cmdPaste | 315 B | 347 B | +10% |

Other functions are **smaller** in tiny because removed indirection simplifies
the control flow for the optimizer:

| Function | full | tiny | Delta |
|----------|-----:|-----:|------:|
| editingPointMove | 1,290 B | 1,178 B | -8.7% |
| editingPointMoveToChar | 563 B | 516 B | -8.4% |
| searchResultNext | 428 B | 370 B | -13.5% |
| searchResultPrev | 448 B | 390 B | -12.9% |
| editorInsertRow | 343 B | 263 B | -23% |

The net effect: app code is roughly similar size, but the massive reduction in
aeolus library overhead (8,248 → 2,809 B) makes tiny+gc ~23% smaller overall.

## What else could improve aeolus

1. **Mark vec functions as `static inline` in tiny** — instead of external
   linkage functions that need gc-sections to strip, put the implementations
   in the header. The compiler inlines them directly with no leftover bodies.
   Downside: slower compilation if many TUs include the header.

2. **Reduce Iterator function count** — the full variant keeps 35 iterator
   functions (5 per type × 7 types). The tiny variant already strips most of
   these (keeps only 8). Consider whether all types truly need iterators, or
   if some types (like Highlight, unsigned int) only ever use direct index access.

3. **Profile-guided optimization (PGO)** — `gcc -fprofile-generate` / `-fprofile-use`
   would let the compiler make better inlining decisions based on actual usage
   patterns rather than heuristics.
