from cryptography.fernet import Fernet

# Генерация ключа для симметричного шифрования
encryption_key = Fernet.generate_key()
cipher_suite = Fernet(encryption_key)

# Функция для шифрования идентификатора избирателя
def encrypt_voter_id(voter_id):
    encrypted_id = cipher_suite.encrypt(voter_id.encode()).decode()
    return encrypted_id

# Структура бюллетеня
class Ballot:
    def __init__(self, encrypted_voter_id, candidates):
        self.encrypted_voter_id = encrypted_voter_id
        self.candidates = candidates

# Функция создания бюллетеня
def create_ballot(voter_id, candidates):
    encrypted_id = encrypt_voter_id(voter_id)
    return Ballot(encrypted_id, candidates)

# Пример использования
candidates = ["Кандидат A", "Кандидат B", "Кандидат C"]
ballot = create_ballot("voter1", candidates)
print("Зашифрованный ID избирателя:", ballot.encrypted_voter_id)
print("Список кандидатов:", ballot.candidates)
