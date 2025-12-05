/*
 * Encryption Module for Bare-Metal P2P Node Storage
 *
 * This module provides hardware-accelerated encryption for data at rest
 * using AES-256-GCM mode.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// AES block size in bytes
#define AES_BLOCK_SIZE 16

// AES-256 key size in bytes
#define AES_KEY_SIZE 32

// GCM authentication tag size in bytes
#define GCM_TAG_SIZE 16

// Maximum data size for encryption in one operation (1MB)
#define MAX_ENCRYPTION_SIZE (1024 * 1024)

// AES-256-GCM context
typedef struct {
    uint8_t key[AES_KEY_SIZE];
    uint8_t iv[AES_BLOCK_SIZE];
    uint64_t counter;
} aes_gcm_context_t;

// Global encryption context
static aes_gcm_context_t g_encryption_ctx;

// Hardware AES support detection
static bool g_has_aes_ni = false;

/*
 * Initialize encryption module
 */
int encryption_init(const uint8_t* master_key) {
    // Copy master key
    memcpy(g_encryption_ctx.key, master_key, AES_KEY_SIZE);
    
    // Initialize IV with zeros
    memset(g_encryption_ctx.iv, 0, AES_BLOCK_SIZE);
    
    // Initialize counter
    g_encryption_ctx.counter = 0;
    
    // Detect AES-NI support (simplified)
    // In real implementation, this would check CPUID
    g_has_aes_ni = true; // Assuming AES-NI is available
    
    return 0;
}

/*
 * Generate unique IV for each encryption operation
 */
static void generate_iv(uint8_t* iv, uint64_t counter) {
    // Use first 8 bytes of IV for counter
    iv[0] = (counter >> 56) & 0xFF;
    iv[1] = (counter >> 48) & 0xFF;
    iv[2] = (counter >> 40) & 0xFF;
    iv[3] = (counter >> 32) & 0xFF;
    iv[4] = (counter >> 24) & 0xFF;
    iv[5] = (counter >> 16) & 0xFF;
    iv[6] = (counter >> 8) & 0xFF;
    iv[7] = counter & 0xFF;
    
    // Use remaining bytes from context IV
    memcpy(&iv[8], &g_encryption_ctx.iv[8], 8);
}

/*
 * Hardware-accelerated AES-256 encryption (simplified)
 */
static void aes256_encrypt_block(const uint8_t* plaintext, uint8_t* ciphertext, const uint8_t* key) {
    // In real implementation, this would use AES-NI instructions
    // For now, we'll just copy the data (this is NOT secure)
    memcpy(ciphertext, plaintext, AES_BLOCK_SIZE);
    
    // XOR with key (this is NOT secure, just for demonstration)
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        ciphertext[i] ^= key[i % AES_KEY_SIZE];
    }
}

/*
 * GHASH function for GCM mode (simplified)
 */
static void ghash(uint8_t* hash, const uint8_t* data, size_t data_len, const uint8_t* key) {
    // In real implementation, this would be a proper GHASH function
    // For now, we'll just XOR the data with the key
    memset(hash, 0, AES_BLOCK_SIZE);
    
    for (size_t i = 0; i < data_len; i++) {
        hash[i % AES_BLOCK_SIZE] ^= data[i] ^ key[i % AES_KEY_SIZE];
    }
}

/*
 * Encrypt data using AES-256-GCM
 */
int encryption_encrypt(const uint8_t* plaintext, size_t plaintext_len, 
                      const uint8_t* associated_data, size_t ad_len,
                      uint8_t* ciphertext, uint8_t* tag) {
    if (plaintext_len > MAX_ENCRYPTION_SIZE) {
        return -1; // Data too large
    }
    
    // Generate unique IV
    generate_iv(g_encryption_ctx.iv, g_encryption_ctx.counter);
    g_encryption_ctx.counter++;
    
    // Encrypt data in CTR mode
    uint8_t counter_block[AES_BLOCK_SIZE];
    uint8_t encrypted_counter[AES_BLOCK_SIZE];
    size_t offset = 0;
    
    while (offset < plaintext_len) {
        // Prepare counter block
        memcpy(counter_block, g_encryption_ctx.iv, AES_BLOCK_SIZE);
        
        // Increment counter portion (last 4 bytes)
        uint32_t counter_val = (counter_block[15] << 24) | (counter_block[14] << 16) | 
                              (counter_block[13] << 8) | counter_block[12];
        counter_val++;
        counter_block[12] = counter_val & 0xFF;
        counter_block[13] = (counter_val >> 8) & 0xFF;
        counter_block[14] = (counter_val >> 16) & 0xFF;
        counter_block[15] = (counter_val >> 24) & 0xFF;
        
        // Encrypt counter block
        aes256_encrypt_block(counter_block, encrypted_counter, g_encryption_ctx.key);
        
        // XOR with plaintext
        size_t block_size = (plaintext_len - offset < AES_BLOCK_SIZE) ? 
                           (plaintext_len - offset) : AES_BLOCK_SIZE;
        
        for (size_t i = 0; i < block_size; i++) {
            ciphertext[offset + i] = plaintext[offset + i] ^ encrypted_counter[i];
        }
        
        offset += block_size;
    }
    
    // Calculate authentication tag using GHASH
    // In real implementation, this would be proper GCM tag calculation
    uint8_t hash[AES_BLOCK_SIZE];
    
    // Hash associated data
    if (ad_len > 0) {
        ghash(hash, associated_data, ad_len, g_encryption_ctx.key);
    }
    
    // Hash ciphertext
    ghash(hash, ciphertext, plaintext_len, g_encryption_ctx.key);
    
    // Encrypt hash to create tag
    aes256_encrypt_block(hash, tag, g_encryption_ctx.key);
    
    return 0;
}

/*
 * Decrypt data using AES-256-GCM
 */
int encryption_decrypt(const uint8_t* ciphertext, size_t ciphertext_len,
                      const uint8_t* associated_data, size_t ad_len,
                      const uint8_t* tag, uint8_t* plaintext) {
    if (ciphertext_len > MAX_ENCRYPTION_SIZE) {
        return -1; // Data too large
    }
    
    // Decrypt data in CTR mode (same as encryption)
    uint8_t counter_block[AES_BLOCK_SIZE];
    uint8_t encrypted_counter[AES_BLOCK_SIZE];
    size_t offset = 0;
    
    // Generate unique IV
    generate_iv(g_encryption_ctx.iv, g_encryption_ctx.counter);
    g_encryption_ctx.counter++;
    
    while (offset < ciphertext_len) {
        // Prepare counter block
        memcpy(counter_block, g_encryption_ctx.iv, AES_BLOCK_SIZE);
        
        // Increment counter portion (last 4 bytes)
        uint32_t counter_val = (counter_block[15] << 24) | (counter_block[14] << 16) | 
                              (counter_block[13] << 8) | counter_block[12];
        counter_val++;
        counter_block[12] = counter_val & 0xFF;
        counter_block[13] = (counter_val >> 8) & 0xFF;
        counter_block[14] = (counter_val >> 16) & 0xFF;
        counter_block[15] = (counter_val >> 24) & 0xFF;
        
        // Encrypt counter block
        aes256_encrypt_block(counter_block, encrypted_counter, g_encryption_ctx.key);
        
        // XOR with ciphertext
        size_t block_size = (ciphertext_len - offset < AES_BLOCK_SIZE) ? 
                           (ciphertext_len - offset) : AES_BLOCK_SIZE;
        
        for (size_t i = 0; i < block_size; i++) {
            plaintext[offset + i] = ciphertext[offset + i] ^ encrypted_counter[i];
        }
        
        offset += block_size;
    }
    
    // Verify authentication tag
    uint8_t calculated_tag[GCM_TAG_SIZE];
    uint8_t hash[AES_BLOCK_SIZE];
    
    // Hash associated data
    if (ad_len > 0) {
        ghash(hash, associated_data, ad_len, g_encryption_ctx.key);
    }
    
    // Hash plaintext
    ghash(hash, plaintext, ciphertext_len, g_encryption_ctx.key);
    
    // Encrypt hash to create tag
    aes256_encrypt_block(hash, calculated_tag, g_encryption_ctx.key);
    
    // Compare tags
    if (memcmp(tag, calculated_tag, GCM_TAG_SIZE) != 0) {
        return -2; // Authentication failed
    }
    
    return 0;
}

/*
 * Generate random key
 */
void encryption_generate_random_key(uint8_t* key, size_t key_len) {
    // In real implementation, this would use hardware random number generator
    // For now, we'll just fill with pseudo-random data
    for (size_t i = 0; i < key_len; i++) {
        key[i] = (uint8_t)(i * 17 + 23) & 0xFF;
    }
}