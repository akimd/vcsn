FROM debian:sid
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

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

RUN useradd -d /vcsn -m -r vcsn

ADD vcsn-2.0a.binary.tar.bz2 /usr/local/stow/
RUN stow -d /usr/local/stow vcsn-2.0a.install                           \
    && ln -s /usr/local/share/doc/vcsn/notebooks /vcsn/Documentation    \
    && touch "/vcsn/Please read the index.ipynb file in Documentation"

EXPOSE 8888

WORKDIR /vcsn
RUN mkdir /vcsn/.ipython                        \
    && chown vcsn:vcsn /vcsn/.ipython
ENV VCSN_DATADIR /vcsn/.ipython

CMD su vcsn -s /bin/bash -c 'IPYTHON=ipython3 vcsn notebook --ip=* --port 8888'
