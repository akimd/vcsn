FROM debian:jessie
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list

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
        libzmq3-dev                                             \
        locales                                                 \
        pdf2svg                                                 \
        python-matplotlib                                       \
        python3-dev                                             \
        python3-pip                                             \
        stow                                                    \
        texlive-latex-extra                                     \
        texlive-pictures                                        \
  && apt-get autoremove                                         \
  && apt-get clean                                              \
  && pip3 install "ipython[notebook]"

# Set the locale
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
  && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# Install vcsn
RUN echo 'deb http://www.lrde.epita.fr/repo/debian/ unstable/' >/etc/apt/sources.list.d/lrde.list       \
    && apt-get update                                                                                   \
    && apt-get install -y --force-yes --no-install-recommends vcsn                                      \
    && useradd -d /vcsn -m -r vcsn                                                                      \
    && mkdir /vcsn/.ipython                                                                             \
    && chown vcsn:vcsn /vcsn/.ipython                                                                   \
    && su vcsn -s /bin/bash -c 'cp -r /usr/share/doc/vcsn/notebooks /vcsn/Doc'                          \
    && ln -s /usr/share/doc/vcsn/notebooks '/vcsn/Doc (read only)'                                      \
    && touch "/vcsn/Please read the index.ipynb file in Doc"

EXPOSE 8888

WORKDIR /vcsn
ENV VCSN_DATADIR /vcsn/.ipython

CMD su vcsn -s /bin/bash -c 'IPYTHON=ipython3 vcsn notebook --ip=* --port 8888'
