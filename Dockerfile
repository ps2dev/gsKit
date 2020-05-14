FROM fjtrujy/ps2dev:ps2sdk-latest

ENV GSKIT=$PS2DEV/gsKit

COPY . /src/gskit

RUN \
  apk add --no-cache --virtual .build-deps gcc musl-dev && \
  cd /src/gskit && \
  make && \
  make install && \
  make clean && \
  apk del .build-deps && \
  rm -rf \
    /src/* \
    /tmp/*

WORKDIR /src
