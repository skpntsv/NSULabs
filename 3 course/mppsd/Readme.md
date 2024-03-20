# Задание

Bash скрипт, который ходит по git-репозиторию, достаёт оттуда все ветки, выбирает оттуда все ветки и выбирает из них те, которые коммитили больше N дней, выводит информацию по ним и пишет человеку на почту что он давно ничего не коммитил

# Настройка

Нам понадобится:

    * Linux (Я использовал WSL)
    * Postfix
    * MailUtils


## 1. Настраиваем сторонний почтовый ящик
Нам нужно получить пароль для приложений.

Я использовал yandex.ru

## 2. Установим необходимое ПО

```bash
sudo apt-get install postfix mailutils
```
Вылаезает окно конфигурации - выбираем "Без настроек"

## 3. Настраиваем конфиги

```bash
sudo vim /etc/postfix/sasl_passwd
```

Пишем:
```vim
smtp.yandex.ru:587 mailbox@yourdomen.ru:your_app_password
```

Назначем права ?зачем? /TODO проверить без них
```bash
chmod 600 /etc/postfix/sasl_passwd
```

Создаём конфигурационный файл Postfix
```bash
nano /etc/postfix/main.cf
```
Куда в последствии вставляем следующие строки
```vim
relayhost = [smtp.yandex.ru]:587

smtp_use_tls = yes

smtp_sasl_auth_enable = yes
smtp_sasl_password_maps = hash:/etc/postfix/sasl_passwd
smtp_sasl_security_options = noanonymous
smtp_sasl_tls_security_options = noanonymous
smtp_tls_CAfile = /etc/ssl/certs/ca-certificates.crt

alias_maps = hash:/etc/aliases
```

Создаем файл-хэш базы данных sasl_passwd.db в каталоге /etc/postfix/ с помощью команды postmap. Этот файл используется для запросов Postfix.
```bash
postmap /etc/postfix/sasl_passwd
```

Перезапускаем сервис Postfix
```bash
systemctl restart postfix.service
```

Проверяем работу Postfix
```bash
sudo service postfix status
```

Производим пробную отправку письма на произвольный почтовый ящик
```bash
echo "Пробный текст в теле письма" | mail -s "Тема тестового сообщения" получатель@mail.ru -aFrom:mailbox@youdomen.ru
```


Спасибо этому сайту и автору Виктору за [статью](https://vicnot.ru/2024/01/26/ubuntu-mail-terminal/)
