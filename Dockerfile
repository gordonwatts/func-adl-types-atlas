# Build the development container

FROM atlas/analysisbase:21.2.184

RUN echo "source ~/release_setup.sh" >> ~/.bashrc
