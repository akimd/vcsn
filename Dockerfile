FROM debian:jessie
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list

# pandas is used in some notebooks (e.g., Expressions.ipynb).
# psutil is used by vcsn ps.
# regex is used by vcsn demangle.
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
        python3-colorama                                        \
        python3-dev                                             \
        python3-matplotlib                                      \
        python3-pandas                                          \
        python3-pip                                             \
        python3-psutil                                          \
        python3-regex                                           \
        texlive-latex-extra                                     \
        texlive-pictures                                        \
        wamerican                                               \
        wamerican-insane                                        \
        wamerican-large                                         \
        wamerican-small                                         \
        wfrench                                                 \
  && apt-get autoremove                                         \
  && apt-get clean                                              \
  && pip3 install jupyter

# Set the locale.
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
  && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# Install vcsn and its dependencies.
RUN echo 'deb http://www.lrde.epita.fr/repo/debian/ unstable/'                  \
           >/etc/apt/sources.list.d/lrde.list                                   \
    && echo 'deb http://www.lrde.epita.fr/repo/debian/ stable/'                 \
             >>/etc/apt/sources.list.d/lrde.list                                \
    && apt-get update                                                           \
    && apt-get install -y --force-yes --no-install-recommends vcsn              \
        libfst3 libfst3-plugins-base libfst-dev libfst-tools                    \
    && jupyter nbextension enable --py --sys-prefix widgetsnbextension          \
    && useradd -d /vcsn -m -r vcsn                                              \
    && mkdir /vcsn/.jupyter                                                     \
    && chown vcsn:vcsn /vcsn/.jupyter                                           \
    && su vcsn -s /bin/bash -c 'cp -r /usr/share/doc/vcsn/notebooks /vcsn/Doc'  \
    && su vcsn -s /bin/bash -c 'vcsn jupyter trust /vcsn/Doc/*'                 \
    && ln -s /usr/share/doc/vcsn/notebooks '/vcsn/Doc (read only)'              \
    && touch "/vcsn/Please read the !Read-me-first.ipynb file in Doc"

EXPOSE 8888

WORKDIR /vcsn
ENV VCSN_DATADIR /vcsn/.jupyter

CMD su vcsn -s /bin/bash -c 'IPYTHON=ipython3 vcsn notebook --ip=* --port 8888'
