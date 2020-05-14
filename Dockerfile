FROM fjtrujy/ps2dev:ps2sdk-latest

ENV GSKIT=$PS2DEV/gsKit

COPY . /src/gskit

RUN \
  cd /src/gskit && \
  make && \
  make install && \
  make clean && \
  rm -rf \
    /src/* \
    /tmp/*

WORKDIR /src
