FROM debian:jessie
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list
COPY 02proxy /etc/apt/apt.conf.d/02proxy

RUN apt-get update                                              \
   && RUNLEVEL=1 DEBIAN_FRONTEND=noninteractive                 \
     apt-get install -y --force-yes --no-install-recommends     \
        ccache                                                  \
        dot2tex                                                 \
        g++                                                     \
        graphviz                                                \
        imagemagick                                             \
        libboost-all-dev                                        \
        libgmp-dev                                              \
        libmagickcore-extra                                     \
        locales                                                 \
        pdf2svg                                                 \
        python-matplotlib                                       \
        python3-all-dev                                         \
        python3.4-dev                                           \
        python3-pip                                             \
        stow                                                    \
        texlive-latex-extra                                     \
        texlive-pictures                                        \
        debhelper                                               \
        autotools-dev                                           \
        bison                                                   \
        flex                                                    \
        doxygen                                                 \
        dh-python                                               \
        build-essential                                         \
        autoconf                                                \
        automake                                                \
        libtool                                                 \
        git                                                     \
        devscripts                                              \
        fakeroot                                                \
  && apt-get autoremove                                         \
  && apt-get clean                                              \

WORKDIR /build
COPY build.sh /build/build.sh
