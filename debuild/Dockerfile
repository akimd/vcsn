FROM debian:sid
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list
COPY 02proxy /etc/apt/apt.conf.d/02proxy

RUN apt-get update                              \
   && RUNLEVEL=1 DEBIAN_FRONTEND=noninteractive \
     apt-get install                            \
        --no-install-recommends                 \
        -y --allow-unauthenticated              \
        autoconf                                \
        automake                                \
        bison                                   \
        build-essential                         \
        ccache                                  \
        debhelper                               \
        devscripts                              \
        dh-python                               \
        dot2tex                                 \
        doxygen                                 \
        fakeroot                                \
        flex                                    \
        g++                                     \
        git                                     \
        graphviz                                \
        imagemagick                             \
        libboost-all-dev                        \
        libgmp-dev                              \
        libtool                                 \
        lintian                                 \
        locales                                 \
        pdf2svg                                 \
        python-matplotlib                       \
        python3-all-dev                         \
        python3.5-dev                           \
        texlive-latex-extra                     \
        texlive-pictures                        \
  && apt-get autoremove                         \
  && apt-get clean

## Debian unstable features 0.5.2, which is broken.  We need
## https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=847099 to be
## fixed.
##
## Yes, the tarball does not match the name of the directory...
ADD https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.5.3.tar.gz /tmp
RUN apt-get update                                      \
    && RUNLEVEL=1 DEBIAN_FRONTEND=noninteractive        \
      apt-get install                                   \
        --no-install-recommends                         \
        -y --allow-unauthenticated                      \
        cmake                                           \
    && cd /tmp/                                         \
    && tar xf yaml-cpp-0.5.3.tar.gz                     \
    && cd yaml-cpp-yaml-cpp-0.5.3                       \
    && cmake                                            \
       -DCMAKE_INSTALL_PREFIX:PATH=/usr .               \
       -DBUILD_SHARED_LIBS=ON                           \
       -DBUILD_STATIC_LIBS=ON                           \
    && make all                                         \
    && make install

# Ccache saves us from useless recompilations.
RUN ccache -M 20G

# Set the locale
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
  && locale-gen
ENV LANG=en_US.UTF-8    \
    LANGUAGE=en_US:en   \
    LC_ALL=en_US.UTF-8

WORKDIR /build
COPY build.sh /build/build.sh
