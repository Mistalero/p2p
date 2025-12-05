// Simple cryptographic functions for bare-metal P2P node

// Generate random bytes
void random_bytes(unsigned char* buffer, unsigned int length) {
    // Simple pseudo-random generator
    static unsigned int seed = 0xDEADBEEF;
    for (unsigned int i = 0; i < length; i++) {
        seed = seed * 1103515245 + 12345;
        buffer[i] = (seed >> 16) & 0xFF;
    }
}

// Simple hash function (not cryptographically secure)
void simple_hash(const unsigned char* data, unsigned int length, unsigned char* hash) {
    unsigned int h = 0x811C9DC5;
    for (unsigned int i = 0; i < length; i++) {
        h ^= data[i];
        h *= 0x01000193;
    }
    
    // Copy hash to output
    for (int i = 0; i < 4; i++) {
        hash[i] = (h >> (i * 8)) & 0xFF;
    }
}

// Generate key pair (simplified)
void generate_keypair(unsigned char* public_key, unsigned char* private_key) {
    // Generate private key
    random_bytes(private_key, 32);
    
    // Generate public key (simplified - just hash private key)
    simple_hash(private_key, 32, public_key);
}

// Sign message (simplified)
void sign_message(const unsigned char* message, unsigned int length, 
                  const unsigned char* private_key, unsigned char* signature) {
    // Simple signature - hash message and private key
    unsigned char combined[64];
    for (int i = 0; i < 32; i++) {
        combined[i] = message[i % length];
    }
    for (int i = 0; i < 32; i++) {
        combined[32 + i] = private_key[i];
    }
    
    simple_hash(combined, 64, signature);
}

// Verify signature (simplified)
int verify_signature(const unsigned char* message, unsigned int length,
                    const unsigned char* signature, const unsigned char* public_key) {
    // Simplified verification - just check if signature matches expected value
    unsigned char expected[32];
    unsigned char combined[64];
    
    for (int i = 0; i < 32; i++) {
        combined[i] = message[i % length];
    }
    for (int i = 0; i < 32; i++) {
        combined[32 + i] = public_key[i]; // Using public key instead of private for verification
    }
    
    simple_hash(combined, 64, expected);
    
    // Compare signatures
    for (int i = 0; i < 32; i++) {
        if (signature[i] != expected[i]) {
            return 0; // Invalid signature
        }
    }
    
    return 1; // Valid signature
}