FROM mongo:6

COPY init-replica.sh /init-replica.sh
RUN chmod +x /init-replica.sh

CMD ["/init-replica.sh"]