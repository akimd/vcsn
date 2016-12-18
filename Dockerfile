FROM debian:sid
MAINTAINER Clément Démoulins <demoulins@lrde.epita.fr>

COPY sources.list /etc/apt/sources.list

# pandas is used in some notebooks (e.g., Expressions.ipynb).
# psutil is used by vcsn ps.
# regex is used by vcsn demangle.
RUN apt-get update                              \
   && RUNLEVEL=1 DEBIAN_FRONTEND=noninteractive \
     apt-get install -y --no-install-recommends \
        ccache                                  \
        dot2tex                                 \
        g++                                     \
        graphviz                                \
        imagemagick                             \
        libboost-all-dev                        \
        libgmp-dev                              \
        libmagickcore-6.q16-2-extra             \
        libzmq3-dev                             \
        locales                                 \
        pdf2svg                                 \
        python3-colorama                        \
        python3-dev                             \
        python3-matplotlib                      \
        python3-pandas                          \
        python3-pip                             \
        python3-psutil                          \
        python3-regex                           \
        python3-setuptools                      \
        texlive-latex-extra                     \
        texlive-pictures                        \
        wamerican                               \
        wamerican-insane                        \
        wamerican-large                         \
        wamerican-small                         \
        wfrench                                 \
  && apt-get autoremove                         \
  && apt-get clean                              \
  && pip3 install jupyter

# Set the locale.
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
  && locale-gen
ENV LANG=en_US.UTF-8   \
    LANGUAGE=en_US:en  \
    LC_ALL=en_US.UTF-8

# Install Tini. Tini operates as a process subreaper for jupyter. This
# prevents kernel crashes.
#
# See http://jupyter-notebook.readthedocs.io/en/latest/public_server.html.
ENV TINI_VERSION=v0.13.2
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /usr/bin/tini
RUN chmod +x /usr/bin/tini
ENTRYPOINT ["/usr/bin/tini", "--"]

# Install vcsn dependencies.
RUN echo 'deb http://www.lrde.epita.fr/repo/debian/ unstable/'                  \
           >/etc/apt/sources.list.d/lrde.list                                   \
    && echo 'deb http://www.lrde.epita.fr/repo/debian/ stable/'                 \
             >>/etc/apt/sources.list.d/lrde.list                                \
    && apt-get update                                                           \
    && apt-get install --no-install-recommends -y --allow-unauthenticated       \
            libfst-dev                                                          \
            libfst-tools                                                        \
            libfst3                                                             \
            libfst3-plugins-base

# Install vcsn.
RUN apt-get install --no-install-recommends -y --allow-unauthenticated       \
            vcsn                                                             \
    && jupyter nbextension enable --py --sys-prefix widgetsnbextension       \
    && useradd -d /vcsn -m -r vcsn


## ----- ##
## Run.  ##
## ----- ##

EXPOSE 8888

# Set up the `vcsn` user.
WORKDIR /vcsn
USER vcsn

# Ccache saves us from useless recompilations.
RUN ccache -M 20G                                               \
    && mkdir /vcsn/.jupyter                                     \
    && cp -r /usr/share/doc/vcsn/notebooks Doc                  \
    && jupyter trust Doc/*                                      \
    && ln -s /usr/share/doc/vcsn/notebooks 'Doc (read only)'    \
    && touch 'Please read the !Read-me-first.ipynb file in Doc'

CMD ["vcsn", "notebook", "--ip=0.0.0.0", "--port", "8888", "--no-browser"]
