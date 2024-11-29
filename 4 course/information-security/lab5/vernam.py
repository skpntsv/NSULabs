import random

def generate_key(length):
    """Генерирует случайный ключ заданной длины"""
    return ''.join(chr(random.randint(0, 255)) for _ in range(length))

def vernam_encrypt_decrypt(text, key):
    """Шифрование/дешифрование методом Вернама"""
    if len(text) != len(key):
        raise ValueError("Длина текста и ключа должны совпадать")
    return ''.join(chr(ord(t) ^ ord(k)) for t, k in zip(text, key))



text = input("Введите текст для шифрования: ")

key = generate_key(len(text))
print(f"Сгенерированный ключ: {key}")

encrypted_text = vernam_encrypt_decrypt(text, key)
print(f"Зашифрованный текст: {encrypted_text}")

decrypted_text = vernam_encrypt_decrypt(encrypted_text, key)
print(f"Расшифрованный текст: {decrypted_text}")
