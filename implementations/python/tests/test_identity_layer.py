"""
Tests for Identity Layer Implementation
"""

import unittest
import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from src.identity_layer import IdentityLayer


class TestIdentityLayer(unittest.TestCase):
    def setUp(self):
        self.identity_layer = IdentityLayer()
        
    def test_generate_nostr_keys(self):
        """Test generating Nostr keys"""
        keys = self.identity_layer.generate_nostr_keys()
        
        # Check that both keys are present
        self.assertIsNotNone(keys['private_key'])
        self.assertIsNotNone(keys['public_key'])
        
        # Check that keys are bytes
        self.assertIsInstance(keys['private_key'], bytes)
        self.assertIsInstance(keys['public_key'], bytes)
        
    def test_create_did_document(self):
        """Test creating DID document"""
        # First generate keys
        self.identity_layer.generate_nostr_keys()
        
        # Create DID document
        did_document = self.identity_layer.create_did_document()
        
        # Check that DID document is created
        self.assertIsNotNone(did_document)
        self.assertIn('id', did_document)
        self.assertIn('verificationMethod', did_document)
        self.assertIn('authentication', did_document)
        self.assertIn('assertionMethod', did_document)
        
        # Check DID format
        self.assertTrue(did_document['id'].startswith('did:key:'))
        
    def test_create_did_document_without_keys(self):
        """Test creating DID document without generating keys first"""
        with self.assertRaises(Exception):
            self.identity_layer.create_did_document()
            
    def test_bind_did_to_nostr(self):
        """Test binding DID to Nostr key"""
        # First generate keys
        keys = self.identity_layer.generate_nostr_keys()
        
        # Create DID document
        did_document = self.identity_layer.create_did_document()
        
        # Bind DID to Nostr key
        binding = self.identity_layer.bind_did_to_nostr(did_document['id'], keys['private_key'])
        
        # Check that binding is created
        self.assertIsNotNone(binding)
        self.assertIn('did', binding)
        self.assertIn('nostr_public_key', binding)
        self.assertIn('timestamp', binding)
        self.assertIn('signature', binding)
        
    def test_resolve_did(self):
        """Test resolving DID"""
        # First generate keys
        self.identity_layer.generate_nostr_keys()
        
        # Create DID document
        did_document = self.identity_layer.create_did_document()
        
        # Resolve DID
        result = self.identity_layer.resolve_did(did_document['id'])
        
        # Check that result is returned
        self.assertIsNotNone(result)
        self.assertIn('did', result)
        self.assertIn('resolved', result)
        self.assertIn('timestamp', result)


if __name__ == '__main__':
    unittest.main()