# Internal Documentation

This directory contains comprehensive internal documentation for the Zelx compiler project. These documents are designed for developers, maintainers, and contributors who need to understand the system at a technical level.

## Documentation Overview

### 📋 [ARCHITECTURE.md](ARCHITECTURE.md)
**System Architecture and Design**

High-level architectural overview of the Zelx compiler system:
- Core components (Frontend, Backend, Runtime)
- Compilation pipeline stages
- Directory structure
- Type system design
- Memory management strategy
- Error handling approach
- Performance considerations
- Dependencies and build systems
- Testing strategy
- Future extension points

**Read this first** to understand the big picture of how the compiler works.

---

### 🔄 [CODE_FLOW.md](CODE_FLOW.md)
**Code Flow and Execution Patterns**

Detailed explanation of how code flows through the system:
- Compiler execution flow
- Lexer → Parser → Codegen pipeline
- Key code patterns and idioms
- Control flow generation
- Function call generation
- Array operations flow
- Build process flow
- Test execution flow
- Error propagation
- Memory allocation flow
- Debugging support
- Extension points for adding features

**Read this** to understand the dynamic behavior and execution patterns.

---

### 👨‍💻 [DEVELOPER_GUIDE.md](DEVELOPER_GUIDE.md)
**Developer Guide and Workflow**

Practical guide for developers working on the codebase:
- Getting started and build instructions
- Code organization and file overview
- Development workflow
- Step-by-step feature addition guide
- Debugging techniques
- Testing strategies
- Code style guidelines
- Common patterns
- Performance considerations
- Troubleshooting guide
- Contributing guidelines

**Read this** when you start working on the codebase or need practical guidance.

---

### 📖 [LANGUAGE_SPEC.md](LANGUAGE_SPEC.md)
**Language Specification**

Complete specification of the Zelx language:
- Lexical structure (tokens, literals, operators)
- Syntax rules (variables, arrays, control flow)
- Built-in functions
- Type system and type coercion
- Semantics and execution model
- Standard library
- Code examples
- Grammar reference
- Implementation notes
- Future features

**Read this** to understand what the Zelx language can do and how to use it.

---

### ⚙️ [IMPLEMENTATION_DETAILS.md](IMPLEMENTATION_DETAILS.md)
**Implementation Details and Technical Decisions**

Deep dive into implementation specifics:
- AST node type system design
- Parser implementation details
- Code generation strategy
- Runtime system design
- Memory management policies
- Error handling strategy
- Build system design
- Testing infrastructure
- Performance considerations
- Debugging support
- Known limitations
- Future implementation plans
- Portability considerations
- Security considerations
- Maintenance guidelines

**Read this** to understand the technical "why" behind implementation decisions.

---

### 🚀 [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
**Quick Reference Guide**

Condensed reference for common tasks:
- Compiler commands
- File structure
- AST node types
- Language syntax quick reference
- Common tasks (adding tokens, operators, functions)
- Error messages
- Code patterns
- Testing procedures
- Performance tips
- Troubleshooting guide
- Key contacts and resources

**Read this** when you need quick answers or reminders while working.

---

## Recommended Reading Order

### For New Contributors
1. **ARCHITECTURE.md** - Understand the system
2. **LANGUAGE_SPEC.md** - Learn the language
3. **DEVELOPER_GUIDE.md** - Start contributing
4. **QUICK_REFERENCE.md** - Keep handy for reference

### For Maintainers
1. **ARCHITECTURE.md** - System overview
2. **IMPLEMENTATION_DETAILS.md** - Technical details
3. **CODE_FLOW.md** - Execution patterns
4. **DEVELOPER_GUIDE.md** - Maintenance procedures

### For Feature Development
1. **LANGUAGE_SPEC.md** - Language capabilities
2. **ARCHITECTURE.md** - System design
3. **CODE_FLOW.md** - Extension points
4. **QUICK_REFERENCE.md** - Common patterns

### For Debugging Issues
1. **QUICK_REFERENCE.md** - Troubleshooting
2. **CODE_FLOW.md** - Flow analysis
3. **IMPLEMENTATION_DETAILS.md** - Technical context
4. **DEVELOPER_GUIDE.md** - Debugging techniques

## Document Maintenance

### When to Update Documentation
- **ARCHITECTURE.md**: Major structural changes, new components
- **CODE_FLOW.md**: New execution patterns, changed flows
- **DEVELOPER_GUIDE.md**: New procedures, changed workflows
- **LANGUAGE_SPEC.md**: Language syntax/feature changes
- **IMPLEMENTATION_DETAILS.md**: Technical decision changes
- **QUICK_REFERENCE.md**: New commands, changed patterns

### Documentation Standards
- Keep technical accuracy
- Provide examples
- Cross-reference related documents
- Update modification dates
- Maintain consistent formatting

## Key Concepts

### Compiler Pipeline
```
.zx source → Lexer → Parser → AST → Codegen → C code → GCC → Executable
```

### Key Components
- **Frontend**: Lexer (Flex) + Parser (Bison)
- **Backend**: Code Generator
- **Runtime**: C helper functions
- **Build System**: build.zx + Makefile

### Important Files
- `src/main.c` - Entry point
- `src/lex.l` - Lexer definition
- `src/parser.y` - Parser grammar
- `src/codegen.c` - Code generation
- `src/codegen.h` - AST structures

## Contribution Guidelines

### Adding New Features
1. Update relevant documentation
2. Add tests
3. Update this README if needed
4. Cross-reference related documents

### Fixing Bugs
1. Document the bug and fix
2. Update troubleshooting sections
3. Add to known issues if applicable
4. Update quick reference if patterns change

### Improving Documentation
1. Keep technical accuracy
2. Add examples where helpful
3. Maintain consistency
4. Update reading order if needed

## Additional Resources

### External Documentation
- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)

### Project Files
- `changelog.txt` - Version history
- `Makefile` - Build system
- `build.zx` - Build automation
- `.gitignore` - Git configuration

## Contact and Support

### For Questions
- Check relevant documentation first
- Review code examples
- Check test cases for usage patterns
- Review implementation details for technical questions

### For Issues
- Document the problem
- Check troubleshooting guides
- Review error messages
- Examine generated code

### For Contributions
- Follow contribution guidelines
- Update documentation
- Add tests
- Maintain code style

## Document Metadata

### Last Updated
- **ARCHITECTURE.md**: 2026-07-11
- **CODE_FLOW.md**: 2026-07-11
- **DEVELOPER_GUIDE.md**: 2026-07-11
- **LANGUAGE_SPEC.md**: 2026-07-11
- **IMPLEMENTATION_DETAILS.md**: 2026-07-11
- **QUICK_REFERENCE.md**: 2026-07-11
- **README.md**: 2026-07-11

### Version
- Zelx Compiler: 0.1.0 (see changelog.txt for current version)

### Status
- Documentation: Complete
- Project: Active development

---

**Note**: This documentation is intended for internal use by developers and maintainers. For user-facing documentation, please refer to the project README and user guides.
