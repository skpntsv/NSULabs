import os
from PIL import Image

def embed_message(image_path, output_path, message):
    image = Image.open(image_path)
    encoded = image.copy()
    width, height = image.size
    message += '###'  # маркер конца сообщения
    binary_message = ''.join([format(ord(i), '08b') for i in message])  # Преобразуем в двоичный вид

    data_index = 0
    for y in range(height):
        for x in range(width):
            pixel = list(image.getpixel((x, y)))
            for i in range(3):
                if data_index < len(binary_message):
                    pixel[i] = (pixel[i] & ~1) | int(binary_message[data_index])
                    data_index += 1
            encoded.putpixel((x, y), tuple(pixel))
            if data_index >= len(binary_message):
                break
        if data_index >= len(binary_message):
            break

    os.makedirs(os.path.dirname(output_path), exist_ok=True)   
    encoded.save(output_path)
    print(f"Сообщение успешно внедрено в {output_path}.")

def extract_message(image_path):
    image = Image.open(image_path)
    width, height = image.size

    binary_message = ""
    for y in range(height):
        for x in range(width):
            pixel = list(image.getpixel((x, y)))
            for i in range(3):
                binary_message += str(pixel[i] & 1)

    all_bytes = [binary_message[i:i+8] for i in range(0, len(binary_message), 8)]
    decoded_message = ""
    for byte in all_bytes:
        decoded_message += chr(int(byte, 2))
        if decoded_message.endswith('###'):  # маркер конца сообщения
            break

    return decoded_message[:-3]  # Убираем маркер конца сообщения


input_image = "resources/image.png"
output_image = os.path.join("out", "output_image_big_message.png")


#message = input("Введите сообщение для внедрения: ")
message = open("../message.txt", 'r').read()

embed_message(input_image, output_image, message)

extracted_message = extract_message(output_image)
print(f"Извлечённое сообщение: {extracted_message}")
