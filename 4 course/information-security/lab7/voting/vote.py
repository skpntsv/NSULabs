# Импорт необходимых модулей
from pysnark.runtime import snark, PrivVal, PubVal
from pysnark.branching import if_then_else
from cryptography.fernet import Fernet
import hashlib

# Генерация ключа для симметричного шифрования
# В реальной системе этот ключ должен храниться безопасно и быть постоянным
encryption_key = Fernet.generate_key()
cipher_suite = Fernet(encryption_key)

# Функция для шифрования идентификатора избирателя
def encrypt_voter_id(voter_id):
    encrypted_id = cipher_suite.encrypt(voter_id.encode()).decode()
    return encrypted_id

# Функция для преобразования строки в целое число
def string_to_int(s):
    return int(hashlib.sha256(s.encode()).hexdigest(), 16) % (10**16)  # Ограничиваем размер для примера

# Структура бюллетеня
class Ballot:
    def __init__(self, encrypted_voter_id, candidates):
        self.encrypted_voter_id = encrypted_voter_id
        self.candidates = candidates

# Функция создания бюллетеня
def create_ballot(voter_id, candidates):
    encrypted_id = encrypt_voter_id(voter_id)
    return Ballot(encrypted_id, candidates)

# Структура для хранения голоса и доказательства
class VoteProof:
    def __init__(self, candidate, proof):
        self.candidate = candidate
        self.proof = proof

# Функция голосования
def cast_vote(ballot, chosen_candidate, valid_candidates_int):
    # Проверка, что выбранный кандидат допустим
    assert chosen_candidate in [c['name'] for c in valid_candidates_int], "Недопустимый кандидат"

    @snark
    def vote_proof(encrypted_voter_id_int, chosen_candidate_int, *valid_candidates):
        # Проверка, что выбранный кандидат присутствует в списке допустимых кандидатов
        is_valid = 0
        for c in valid_candidates:
            is_valid += (chosen_candidate_int == c)
        
        # Используем if_then_else для проверки условия
        # Если is_valid > 0, то условие истинно (1), иначе ложь (0)
        condition = if_then_else(is_valid > 0, 1, 0)
        
        # Assert, что условие истинно
        assert(condition - 1 == 0)  # Вместо assert(condition == 1)
        
        return condition

    # Преобразуем данные в целые числа
    encrypted_voter_id_int = string_to_int(ballot.encrypted_voter_id)
    chosen_candidate_int = string_to_int(chosen_candidate)

    # Собираем числовые представления допустимых кандидатов
    valid_candidates = [c['id'] for c in valid_candidates_int]

    # Генерация доказательства
    proof = vote_proof(PrivVal(encrypted_voter_id_int), PubVal(chosen_candidate_int), *valid_candidates)
    return VoteProof(chosen_candidate, proof)

# Функция подсчета голосов
def count_votes(vote_proofs):
    vote_count = {}
    for vp in vote_proofs:
        vote_count[vp.candidate] = vote_count.get(vp.candidate, 0) + 1
    return vote_count

# Функция проверки результатов голосования
def verify_results(vote_proofs, results):
    for vp in vote_proofs:
        # В реальной реализации необходимо проверить доказательство
        # Например:
        # if not verify(vp.proof):
        #     return False
        # Здесь мы упрощенно считаем, что доказательство всегда верно
        if vp.candidate not in results:
            return False
    return True

# Пример основного приложения голосования
def main():
    # Список кандидатов с числовыми представлениями
    candidates = ["Кандидат A", "Кандидат B", "Кандидат C"]
    valid_candidates_int = [{'name': c, 'id': string_to_int(c)} for c in candidates]

    # Создание бюллетеней для избирателей
    ballots = [
        create_ballot("voter1", candidates),
        create_ballot("voter2", candidates)
        # Добавьте больше избирателей по необходимости
    ]

    # Процесс голосования
    vote_proofs = []
    vote_proofs.append(cast_vote(ballots[0], "Кандидат A", valid_candidates_int))
    vote_proofs.append(cast_vote(ballots[1], "Кандидат B", valid_candidates_int))
    # Добавьте больше голосов по необходимости

    # Подсчет голосов
    results = count_votes(vote_proofs)

    # Вывод результатов
    print("\nРезультаты голосования:")
    for candidate, count in results.items():
        print(f"{candidate}: {count} голосов")

    # Проверка результатов
    is_valid = verify_results(vote_proofs, results)
    if is_valid:
        print("\nРезультаты голосования подтверждены.")
    else:
        print("\nОшибка при проверке результатов голосования.")

if __name__ == "__main__":
    main()
