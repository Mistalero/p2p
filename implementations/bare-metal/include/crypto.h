#ifndef CRYPTO_H
#define CRYPTO_H

// Generate random bytes
void random_bytes(unsigned char* buffer, unsigned int length);

// Simple hash function
void simple_hash(const unsigned char* data, unsigned int length, unsigned char* hash);

// Generate key pair
void generate_keypair(unsigned char* public_key, unsigned char* private_key);

// Sign message
void sign_message(const unsigned char* message, unsigned int length, 
                  const unsigned char* private_key, unsigned char* signature);

// Verify signature
int verify_signature(const unsigned char* message, unsigned int length,
                    const unsigned char* signature, const unsigned char* public_key);

#endif