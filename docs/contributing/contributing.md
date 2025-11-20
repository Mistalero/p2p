# Contributing to the P2P Node Project

We welcome contributions to the P2P Node project! This document explains how you can contribute.

## Code of Conduct

Please read and follow our [Code of Conduct](code-of-conduct.md) to ensure a welcoming environment for all contributors.

## How to Contribute

### Reporting Issues

If you find a bug or have a feature request:

1. Check if the issue already exists in our [issue tracker](../../issues)
2. If not, create a new issue with:
   - A clear title and description
   - Steps to reproduce (for bugs)
   - Expected and actual behavior
   - System information (OS, Node.js version, etc.)

### Submitting Pull Requests

1. Fork the repository
2. Create a new branch for your feature or bug fix
3. Make your changes
4. Add or update tests as needed
5. Ensure all tests pass
6. Commit your changes with a clear commit message
7. Push to your fork
8. Create a pull request to the `main` branch

### Code Style

- Follow the existing code style in the project
- Use clear, descriptive variable and function names
- Add comments for complex logic
- Keep functions small and focused on a single task

### Testing

- Write unit tests for new functionality
- Ensure all existing tests pass before submitting
- Add integration tests for significant changes

## Development Setup

1. Clone the repository:
   ```bash
   git clone https://git.sourcecraft.dev/p2p-decentralized-emulation/p2p.git
   cd p2p/implementations/javascript
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Run tests:
   ```bash
   npm test
   ```

4. Start the development server:
   ```bash
   npm start
   ```

## Adding New Language Implementations

To add support for a new programming language:

1. Create a new directory under `/implementations` with your language name
2. Follow the protocol specification in `/spec/protocol.md`
3. Implement all core components:
   - Event log storage
   - TCP server for connections
   - Key pair generation
   - Event log merging
   - Event broadcasting
4. Ensure compatibility with existing implementations
5. Add documentation for your implementation

## Documentation Contributions

To contribute to documentation:

1. Follow the same pull request process as code contributions
2. Use clear, concise language
3. Include examples where appropriate
4. Follow the existing documentation structure
5. Update the table of contents if adding new pages

## Getting Help

If you need help with your contribution:

1. Join our community chat (link in README)
2. Ask questions in relevant issues
3. Contact maintainers directly

Thank you for contributing to the P2P Node project!