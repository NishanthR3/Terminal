FROM gcc:7.5

WORKDIR /usr/src/app

COPY src ./src

COPY Makefile ./Makefile

RUN make build

CMD ["make", "run"]