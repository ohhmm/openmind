# Valuable Hamiltonian Code Generation

Design: generate sparse Hamiltonian terms from a `Valuable` expression the same way
`OpenCL()` and `Asm()` generate code.

## Public API
```cpp
struct HamiltonianTerm { size_t i; size_t j; double value; };

struct HamiltonianFile {
    std::string file_name;
    size_t num_variables;
    std::vector<HamiltonianTerm> terms;
};

class Valuable {
public:
    std::vector<HamiltonianTerm> HamiltonianTerms() const;
    HamiltonianFile ToHamiltonianFile() const;
};
```

## Algorithm
1. `Vars()` defines matrix indices.
2. Recursive traversal expands products into monomials.
3. Coefficients accumulate into `(i,j)` entries.
4. Symmetric quadratic terms produce both `(i,j)` and `(j,i)`.
5. Linear terms populate diagonal entries.
6. Constant part is retained as an offset.

## QCI integration
- Uploader converts `HamiltonianTerms` into `file_config.hamiltonian.data`.
- The same metadata/content upload protocol used by the Dirac benchmark is reused.
- `Valuable::OpenCL()` remains the reference example for expression traversal.
