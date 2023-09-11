# Протокол группы 21210 *M.U.D.A.K*

> Multicast\
Udp\
Datagram\
&\
Address\
Knowledge


1. MUDAK Report 
   - при join группы
   - query каждые N секунд

2. MUDAK Leave:
   - при выходе из группы

3. type[`byte`]
   - 1 - Report
   - 2 - Leave

У каждого есть таблица с айпишниками, результиурем таблицу раз в 2N

# Требования:
## N = 10 секунд