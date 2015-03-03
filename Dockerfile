# FROM ipython/notebook
FROM debian:sid
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

RUN apt-get update && apt-get install -y \
        ccache \
        g++ \
        ipython-notebook \
        libboost-all-dev \
        libgmp-dev \
        doxygen \
        graphviz \
        python-matplotlib \
        stow \
        && apt-get autoremove \
        && apt-get clean

RUN useradd -d /vcsn -m -r vcsn

ADD vcsn-2.0a.binary.tar.bz2 /usr/local/stow/
RUN stow -d /usr/local/stow vcsn-2.0a.install
RUN ln -s /usr/local/share/doc/vcsn/notebooks /vcsn/

EXPOSE 8888

WORKDIR /vcsn
RUN mkdir /vcsn/.ipython
RUN chown vcsn:vcsn /vcsn/.ipython
ENV VCSN_DATADIR /vcsn/.ipython

CMD su vcsn -s /bin/bash -c 'vcsn-notebook --ip=* --port 8888'

