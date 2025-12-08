# Key Concepts

## Artifact as Code, State, and Possibility

An artifact is a self-contained unit that contains:

- **Code**: The executable logic of the component.
- **State**: The initial state or state schema of the component.
- **Possibility**: The potential future behavior of the component.

An artifact is not just data - it is a living part of the system that can be instantiated, executed, and evolved.

## Artifact Formats

An artifact must be presented in one of two formats:

### C-Compatible Source Fragment

A C-compatible source fragment is a piece of C code that:

- Has explicit memory management (no malloc, free, etc.)
- Has no external dependencies (no standard library, no OS services)
- Can be compiled with a freestanding C compiler
- Uses only the functions and data structures provided by the kernel

Example:
```c
// Example C-compatible source fragment
void my_component_init() {
    // Initialize the component
}

void my_component_process() {
    // Process the component's logic
}

// Component data
static unsigned char my_component_data[1024];
```

### Forth Word

A Forth word is a sequence of Forth words that:

- Use only words already known to the system or explicitly defined by the artifact
- Are stored as bytecode and dictionary entries, visible in memory
- Can be executed, modified, copied, analyzed, and transmitted to other nodes as-is

Example:
```forth
: my-component-init ( -- )
    ." Initializing my component" cr
;

: my-component-process ( -- )
    ." Processing my component" cr
;

: my-component ( -- )
    my-component-init
    my-component-process
;
```

## Integration into Build Process

An artifact is not compiled separately. Instead, it is embedded into the system's build process and becomes part of a single executable image. This means that:

- The artifact's code is compiled and linked with the kernel
- The artifact's state is allocated in the kernel's memory space
- The artifact's functions are available to other components

## Collective Memory Space

If a component requires data storage, it must use the **collective memory space**, organized as a hierarchy of paths, where each path is not a file, but a **point of agreement between participants**.

- Reading from this space is a request to the network
- Writing is an offer that other nodes can accept, reject, or transform
- The component does not manage this data - it merely participates in its life

## Procedural Narrative

If a component generates events, it must do so through the **procedural narrative mechanism**, which does not call functions, but **makes changes to the global state**, marked with timestamps, digital signatures, and context.

- These changes become part of the history, available to all
- They can be restored, reinterpreted, or continued by others

## Transparency and Verifiability

A component must not contain hidden logic: everything it does must be visible during reverse engineering. This requirement is implemented technically: during the build, the system automatically generates a **memory map**, in which every function, structure, and buffer is annotated, and this map is embedded in the image.

- Anyone who receives your component - even in binary form - must be able to restore its logic without having the source code

## Forth Integration

If a component uses Forth, it must be written as a sequence of words, each of which is either already known to the system or explicitly defined by you.

- Forth words are not compiled into machine code - they remain as bytecode and dictionary entries, visible in memory
- This allows not only executing them but also **modifying on the fly**, **copying**, **analyzing**, and **transmitting to other nodes as-is**

## No Monopoly

A component has no right to a monopoly: it cannot capture resources, block access, hide its state, or prevent deletion.

- It exists as long as other network participants consider it useful
- If a component stops participating, its data stops replicating, its code stops being called, and it disappears naturally, without forced removal

## Expanding the Space of the Possible

By creating a component, you do not add functionality - you **expand the space of the possible** for everyone.

- You do not provide a service - you become part of the environment

## Documentation as Understandability

Most importantly: you should not write documentation for the component.

- If its logic is not obvious from its structure, from the memory map, from its behavior in the network, and from its representation during reverse engineering - it is written incorrectly
- Documentation is not text, but **the system itself in a state of being understood**