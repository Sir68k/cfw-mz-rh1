// Impements Sony's hashing scheme
// basically just repeated DES encrypting/xoring

const HASH_KEY = CryptoJS.enc.Hex.parse("0123456789abcdef");

function wordArrayFromBytes(u8) {
    return CryptoJS.lib.WordArray.create(u8);
}

function bytesFromWordArray(wordArray) {
    const words = wordArray.words;
    const sigBytes = wordArray.sigBytes;
    const u8 = new Uint8Array(sigBytes);
    for (let i = 0; i < sigBytes; i++) {
        u8[i] = (words[i >>> 2] >>> (24 - (i % 4) * 8)) & 0xff;
    }
    return u8;
}

function xor8(a, b) {
    const out = new Uint8Array(8);
    for (let i = 0; i < 8; i++) out[i] = a[i] ^ b[i];
    return out;
}

function desEcbEncrypt(blockBytes) {
    const encrypted = CryptoJS.DES.encrypt(
        wordArrayFromBytes(blockBytes),
        HASH_KEY,
        {
            mode: CryptoJS.mode.ECB,
            padding: CryptoJS.pad.NoPadding
        }
    );
    return bytesFromWordArray(encrypted.ciphertext);
}

function sonyHash(dataBytes) {
    let iv = new Uint8Array(8);

    const fullLen = Math.floor(dataBytes.length / 8) * 8;

    for (let i = 0; i < fullLen; i += 8) {
        const block = dataBytes.slice(i, i + 8);
        iv = desEcbEncrypt(xor8(iv, block));
    }

    // Return last 4 bytes
    return iv.slice(4, 8);
}