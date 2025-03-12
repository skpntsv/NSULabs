import struct

# Инициализация начальных значений для SHA-1
h0 = 0x67452301
h1 = 0xEFCDAB89
h2 = 0x98BADCFE
h3 = 0x10325476
h4 = 0xC3D2E1F0

#Добавление отступов (padding) к входным данным
def padding(input_bytes):
    length = len(input_bytes)
    
    # Длина дополнительного заполнения
    padding_length = (56 - (length + 1) % 64) % 64

    # Добавляем бит '1' (0x80)
    padded = input_bytes + b'\x80'

    # Добавляем нули
    padded += b'\x00' * padding_length

    # Добавляем длину сообщения в битах (64 бита)
    bit_length = length * 8
    padded += struct.pack('>Q', bit_length)  # Записываем в big-endian формате

    return padded

#Обработка одного 512-битного блока
def process_block(block):
    global h0, h1, h2, h3, h4

    # Формируем массив из 16 слов (по 32 бита)
    w = list(struct.unpack('>16L', block))

    # Расширяем до 80 слов
    for i in range(16, 80):
        w.append(((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]) << 1) & 0xFFFFFFFF)

    # Инициализируем переменные для обработки
    a, b, c, d, e = h0, h1, h2, h3, h4

    for i in range(80):
        if i < 20:
            f = (b & c) | (~b & d)
            k = 0x5A827999
        elif i < 40:
            f = b ^ c ^ d
            k = 0x6ED9EBA1
        elif i < 60:
            f = (b & c) | (b & d) | (c & d)
            k = 0x8F1BBCDC
        else:
            f = b ^ c ^ d
            k = 0xCA62C1D6

        temp = ((a << 5) & 0xFFFFFFFF) + f + e + k + w[i]
        e = d
        d = c
        c = (b << 30) & 0xFFFFFFFF
        b = a
        a = temp & 0xFFFFFFFF

    h0 = (h0 + a) & 0xFFFFFFFF
    h1 = (h1 + b) & 0xFFFFFFFF
    h2 = (h2 + c) & 0xFFFFFFFF
    h3 = (h3 + d) & 0xFFFFFFFF
    h4 = (h4 + e) & 0xFFFFFFFF

def sha1(input_bytes):
    global h0, h1, h2, h3, h4

    # Сбрасываем начальные значения
    h0 = 0x67452301
    h1 = 0xEFCDAB89
    h2 = 0x98BADCFE
    h3 = 0x10325476
    h4 = 0xC3D2E1F0

    # Добавляем padding
    padded_input = padding(input_bytes)

    # Обрабатываем каждый 512-битный блок
    for i in range(0, len(padded_input), 64):
        process_block(padded_input[i:i+64])

    # Формируем итоговый хэш
    return struct.pack('>5L', h0, h1, h2, h3, h4)

message = b"kikite"
digest = sha1(message)
print(digest.hex())
