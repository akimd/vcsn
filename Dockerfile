FROM debian:sid
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

RUN apt-get update                              \
  && apt-get install -y                         \
        ccache                                  \
        dot2tex                                 \
        g++                                     \
        graphviz                                \
        imagemagick                             \
        libboost-all-dev                        \
        libgmp-dev                              \
        libmagickcore-extra                     \
        pdf2svg                                 \
        python-matplotlib                       \
        python3-dev                             \
        python3-pip                             \
        stow                                    \
        texlive-latex-extra                     \
        texlive-pictures                        \
  && apt-get autoremove                         \
  && apt-get clean                              \
  && pip3 install "ipython[notebook]"

RUN useradd -d /vcsn -m -r vcsn

ADD vcsn-2.0a.binary.tar.bz2 /usr/local/stow/
RUN stow -d /usr/local/stow vcsn-2.0a.install
RUN ln -s /usr/local/share/doc/vcsn/notebooks /vcsn/
RUN touch "/vcsn/Please read the index.ipynb file in Documentation"

EXPOSE 8888

WORKDIR /vcsn
RUN mkdir /vcsn/.ipython
RUN chown vcsn:vcsn /vcsn/.ipython
ENV VCSN_DATADIR /vcsn/.ipython

CMD su vcsn -s /bin/bash -c 'IPYTHON=ipython3 vcsn notebook --ip=* --port 8888'
