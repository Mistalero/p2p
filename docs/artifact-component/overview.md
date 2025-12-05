# Artifact-Based Component Architecture Overview

An artifact-based component is not a separate module, plugin, service, or application, but a contribution to the overall structure of the system. It must be compatible with the system's ontology, verifiable in its logic, and capable of replication through its protocol.

To create such a component, you do not write a "program" but form an **artifact** that is simultaneously code, state, and possibility. This artifact must be presented in one of two permitted formats: either as a **C-compatible source fragment with explicit memory and no external dependencies**, or as a **Forth word embeddable in the node's current dictionary**.

You do not compile the component separately — you embed it into the system's build process, and it becomes part of a single executable image. This means that your component must be able to:

- Initialize without constructors
- Work without malloc
- Not use global variables with implicit initialization
- Not rely on load order
- Not assume the presence of OS services

Everything available to you is physical memory, interrupts, network registers, and functions already present in the kernel.