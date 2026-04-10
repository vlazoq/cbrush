# Architecture — Local File Search Engine

```
┌────────────┐   ┌────────────┐   ┌────────────────┐   ┌────────────┐
│   Walker   │──▶│ Tokenizer  │──▶│ InvertedIndex  │──▶│IndexStore  │
│ (fs crawl) │   │ (text→tok) │   │ (term→postings)│   │ (save/load)│
└────────────┘   └────────────┘   └────────────────┘   └────────────┘
                                          ▲
                                          │
                                  ┌───────┴────────┐
                                  │  QueryEngine   │
                                  │  (AND/glob)    │
                                  └────────────────┘
                                          ▲
                                  ┌───────┴────────┐
                                  │      CLI       │
                                  └────────────────┘
```

Five independent modules, each < 300 lines, each testable in isolation:

| Module         | Owns                                  | Depends on       |
|----------------|---------------------------------------|------------------|
| `walker`       | filesystem iteration, exclusion rules | `<filesystem>`   |
| `tokenizer`    | text → normalized tokens              | nothing          |
| `inverted_index`| in-memory posting lists              | `tokenizer`      |
| `index_store`  | binary file format, persistence       | `inverted_index` |
| `query_engine` | term AND, glob filter, ranking        | `inverted_index` |
| `cli`          | argument parsing, wiring              | all of the above |

## Alternatives considered

- **SQLite FTS5** — reject. The whole point is to learn how inverted indexes
  work, not to hide them.
- **Trigram index instead of inverted** — reject for v1, revisit in project 07.
  Inverted indexes with whole tokens are easier to reason about at this
  experience level.
- **JSON on disk** — reject. Binary is five lines of code, parses 30× faster,
  and is a better learning experience.
