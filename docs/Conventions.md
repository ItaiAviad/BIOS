# C Coding Conventions

---

## General Formatting

- **Indentation:** Use 4 spaces per indentation level.
- **Spaces:** 
  - After keywords like `if`, `for`, `while` (e.g., `if (condition)`).
  - Around operators (`=`, `+`, `-`, etc.).
  - After commas in parameter lists.
  - **Examples:**
      - `if (a == b)`
      - `['a', 'b', 'c']`

---

## Naming Conventions

### Variables

- Use **lowercase** letters.
- Separate words with **underscores** (`_`).
- Global variables should start with `g_`.

#### Examples

- Local variable: `int first_variable;`
- Global variable: `int g_variable;`

### Functions

- Use **lowercase** letters.
- Separate words with **underscores** (`_`).

#### Examples

- `int calculate_sum();`

---

## Comments (mandatory)

### Doxygen Comments (optional)

- Use **Doxygen** comments for functions.
- Place comments directly above the function definition.
- Start with `/**`.
- Use `@brief` for a brief description.
- Use `@param` to describe parameters.
- Use `@return` to describe the return value (if any).

