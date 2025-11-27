// KeyManager.js
// Менеджер ключей для P2P-ноды

import crypto from 'crypto';

class KeyManager {
  constructor() {
    this.privateKey = null;
    this.publicKey = null;
  }

  // Генерация пары ключей
  generateKeyPair() {
    // Генерируем пару ключей RSA
    const { publicKey, privateKey } = crypto.generateKeyPairSync('rsa', {
      modulusLength: 2048,
      publicKeyEncoding: {
        type: 'spki',
        format: 'pem'
      },
      privateKeyEncoding: {
        type: 'pkcs8',
        format: 'pem'
      }
    });

    this.publicKey = publicKey;
    this.privateKey = privateKey;
    
    return {
      publicKey: this.publicKey,
      privateKey: this.privateKey
    };
  }

  // Получение публичного ключа
  getPublicKey() {
    return this.publicKey;
  }

  // Получение приватного ключа
  getPrivateKey() {
    return this.privateKey;
  }

  // Подпись данных
  signData(data) {
    if (!this.privateKey) {
      throw new Error('Private key not generated');
    }

    const signature = crypto.sign('sha256', Buffer.from(data), {
      key: this.privateKey,
      padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
    });

    return signature.toString('base64');
  }

  // Проверка подписи
  verifySignature(data, signature, publicKey) {
    try {
      const verified = crypto.verify(
        'sha256',
        Buffer.from(data),
        {
          key: publicKey,
          padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
        },
        Buffer.from(signature, 'base64')
      );

      return verified;
    } catch (error) {
      return false;
    }
  }
}

export default KeyManager;