# Game Modules System

## Overview

This directory contains game modules that can be loaded into the P2P node as external plugins. These modules are not included in the main repository to maintain modularity and allow for independent development and licensing.

## Module Structure

Each game module should follow this structure:

```
module-name/
├── package.json
├── index.js
├── config/
│   └── module.json
├── src/
│   ├── game-logic.js
│   └── network-handlers.js
└── README.md
```

## Loading Modules

Modules are loaded at runtime through the plugin system. To load a module:

1. Install the module package: `npm install @game-modules/module-name`
2. Add it to the modules configuration in `config/modules.json`
3. Restart the node

## Security

Game modules are protected and not included in the main repository fork. They may require separate licensing or authentication to access.

## Creating New Modules

To create a new game module:

1. Fork the [game-module-template](https://github.com/p2p-decentralized-emulation/game-module-template) repository
2. Implement your game logic in the `src/` directory
3. Define network message handlers for game state synchronization
4. Package and publish your module to npm
5. Submit a pull request to add your module to the list of available modules