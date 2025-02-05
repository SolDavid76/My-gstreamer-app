FROM ubuntu:22.04

# Installer les dépendances
RUN apt update
RUN apt install -y g++ \
    make \
    pkg-config \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    libglib2.0-dev \
    && rm -rf /var/lib/apt/lists/*

# Définir le dossier de travail
WORKDIR /app

# Copier les fichiers source et Makefile
COPY . .

# Compiler ton application avec Makefile
RUN make

# Exécuter l'application par défaut
# CMD ["./GStreamerApp"]
CMD ["/bin/bash"]
