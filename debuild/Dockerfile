FROM debian:jessie
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list
COPY 02proxy /etc/apt/apt.conf.d/02proxy

RUN apt-get update                                              \
   && RUNLEVEL=1 DEBIAN_FRONTEND=noninteractive                 \
     apt-get install -y --force-yes --no-install-recommends     \
        autoconf                                                \
        automake                                                \
        autotools-dev                                           \
        bison                                                   \
        build-essential                                         \
        ccache                                                  \
        debhelper                                               \
        devscripts                                              \
        dh-python                                               \
        dot2tex                                                 \
        doxygen                                                 \
        fakeroot                                                \
        flex                                                    \
        g++                                                     \
        git                                                     \
        graphviz                                                \
        imagemagick                                             \
        libboost-all-dev                                        \
        libgmp-dev                                              \
        libmagickcore-extra                                     \
        libtool                                                 \
        locales                                                 \
        pdf2svg                                                 \
        python-matplotlib                                       \
        python3-all-dev                                         \
        python3-pip                                             \
        python3.4-dev                                           \
        texlive-latex-extra                                     \
        texlive-pictures                                        \
  && apt-get autoremove                                         \
  && apt-get clean

# Set the locale
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
  && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

WORKDIR /build
COPY build.sh /build/build.sh
