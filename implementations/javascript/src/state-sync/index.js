// Index file for state-sync module
// This file exports all CRDT implementations

import CRDT from './CRDT.js';
import LWWRegister from './LWWRegister.js';
import ORSet from './ORSet.js';

export {
  CRDT,
  LWWRegister,
  ORSet
};

export default {
  CRDT,
  LWWRegister,
  ORSet
};