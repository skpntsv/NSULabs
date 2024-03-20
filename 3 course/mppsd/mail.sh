#!/bin/bash

recipient="*@gmail.com"
subject="O my god"

body="Я в шоке ребята!!!!!"
echo -E "$body" | mailx -s "$subject" "$recipient" -aFrom:*.n@yandex.ru